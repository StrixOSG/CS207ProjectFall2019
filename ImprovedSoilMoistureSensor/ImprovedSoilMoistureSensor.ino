/*

Project Summary:
This program will read the analog moisture level of
2 capactive soil moisture sensors, and a digital
temperature and humidity sensor. It then outputs
these readings to a touch screen, and there are buttons
to switch bettween the two plants (soil moisture sensors).
A servo is also attached that will tip a cup to fill one
of the plants with water if it needs it.

Parts:
- 2 x Soil moisture sensors
- 1 x 3.2" TFT Resistive Touchscreen
- 1 x DH11 Temp & Humidity Sensor
- 1 x SG90 Servo

Connections: 
- Touchscreen (8-bit mode):
    D0,XP to digital pin 22
    D1,XM to digital pin 23
    D2 to digital pin 24
    D3 to digital pin 25
    D4 to digital pin 26
    D5 to digital pin 27
    D6 to digital pin 28
    D7 to digital pin 29
    C/D,YP to analog pin 3
    CS,XM to analog pin 2
    WR to analog pin 1
    RD to analog pin 0
    RST to reset pin
    
- Soil Moisture Sensors to anlog pins 6 and 7
- Temp & Humidity Sensor to digital pin 2
- Servo to digital pin 3


Some code copied from: 
  Soil moisture sensor, mapping different moisture levels
  https://github.com/GadgetReboot/Arduino/blob/master/Uno/SSD1306_Soil_Meter/SSD1306_Soil_Meter.in
  Code by GitHub user: GadgetReboot

  Basic touchscreen code, setup, initializattion, etc.
  https://github.com/adafruit/TFTLCD-Library
  Code by: Adafruit

Modified by: Matthew Hamilton
Date Modified: Nov.10, 2019
*/

/////////////////////   BEGIN TOUCHSCREEN   /////////////////////////////////////////
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3               // must be an analog pin, use "An" notation!
#define XM A2               // must be an analog pin, use "An" notation!
#define YM 23               // can be a digital pin
#define XP 22               // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40

/////////////////////   END TOUCHSCREEN   ///////////////////////////////////////////

/////////////////////   BEGIN SOIL MOISTURE SENSORS   ///////////////////////////////

const int SOIL_SENSOR_AMT = 2;
const int SOIL_SENSOR_PINS[] = {A6, A7};  // soil moisture sensor pins
int validSoilSensorReadings[] = {0, 0};           // valid sensor analog reading to record
int soilSensorResults[] = {0, 0};                 // scaled sensor data [0..3] = [wet, damp, moist, dry]

const int wetProbe      = 300;                // wet readings are around 1.5v or analog input value ~300
const int dryProbe      = 620;                // dry readings are around 3v or analog input value ~620

/////////////////////   END SOIL MOISTURE SENSORS   /////////////////////////////////

/////////////////////   BEGIN TEMPERATURE & HUMIDITY SENSORS   /////////////////////

#include "DHT.h"

#define TH_PIN 2
#define TH_TYPE DHT11

DHT dht(TH_PIN, TH_TYPE);

float humidity, celcius, fahrenheit, heatIndexC, heatIndexF;

/////////////////////    END TEMPERATURE & HUMIDITY SENSORS   ////////////////////

/////////////////////   BEGIN SERVO   ////////////////////////////////////////////

#include <Servo.h>

#define SERVO_PIN 3
int angle = 10;

Servo servo;

/////////////////////    END SERVO   /////////////////////////////////////////////

void setup() {
  
  Serial.begin(9600);

  SetupTouchscreen();

  //Setup servo
  //servo.attach(SERVO_PIN);

  //Setup temp & humidity sensor
  dht.begin();
  
}

void SetupTouchscreen(){

  //Clear the touchscreen display
  tft.reset();

  uint16_t identifier = tft.readID();

  //Determine the type of touchscreen and if it's setup correctly
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  
  //Portrait
  tft.setRotation(2);

  //Screen background color
  tft.fillScreen(BLACK);
  
}

void loop() {

  GetSoilMoistureSensorData();
  GetTHSensorData();
  //TestServo();
  DisplayDataOnTouchScreen();

}

void DisplayDataOnTouchScreen(){

  tft.setCursor(0, 0);
  tft.setTextColor(BLUE, BLACK);
  tft.setTextSize(2);
  DisplaySoilMoistureData();
  tft.println("\n");
  tft.setTextColor(WHITE, BLACK);
  DisplayHumidityData();
  DisplayTempData();
  
}

void DisplaySoilMoistureData(){

  tft.println("Moisture Sensors: ");

  for(int i = 0; i < SOIL_SENSOR_AMT; i++){
  
    //Output values to Serial Log
    tft.print(F("Sensor  "));
    tft.print(i + 1);
    tft.print(F(": "));
    DisplaySoilSensorResultOnScreen(soilSensorResults[i]);

  }
  
}

void DisplayTempData(){

  tft.print("Temperature: ");
  DisplayCelcius();
  //DisplayFahrenheit();
  
}

void DisplayCelcius(){

  tft.print(celcius);
  tft.print((char)247); //Degrees symbol
  tft.println("C ");
  
}

void DisplayFahrenheit(){

  tft.print(fahrenheit);
  tft.print((char)247); //Degrees symbol
  tft.println("F");
  
}

void DisplayHumidityData(){

  tft.print("Humidity: ");
  tft.print(humidity);
  tft.println("% ");
  
}

void DisplayHeatIndexData(){
  
  tft.print("Heat index: ");
  tft.print(heatIndexC);
  tft.print((char)247); //Degrees symbol
  tft.println("C ");
  tft.print(heatIndexF);
  tft.print((char)247); //Degrees symbol
  tft.println("F");
  
}

void TestServo(){

  // scan from 0 to 180 degrees
  for(angle = 10; angle < 180; angle++)  
  {                                  
    servo.write(angle);               
    delay(15);                   
  } 
  // now scan back from 180 to 0 degrees
  for(angle = 180; angle > 10; angle--)    
  {                                
    servo.write(angle);           
    delay(15);       
  } 
  
}

void GetTHSensorData(){

  humidity = dht.readHumidity();
  celcius = dht.readTemperature();
  fahrenheit = dht.readTemperature(true);

   if (isnan(humidity) || isnan(celcius) || isnan(fahrenheit)) {
    
    Serial.println(F("Error getting readings from DHT11 sensor!"));
    return;
    
  }

  heatIndexC = dht.computeHeatIndex(celcius, humidity, false);
  heatIndexF = dht.computeHeatIndex(fahrenheit, humidity);

  /*Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(celcius);
  Serial.print(F("°C "));
  Serial.print(fahrenheit);
  Serial.print(F("°F  Heat index: "));
  Serial.print(heatIndexC);
  Serial.print(F("°C "));
  Serial.print(heatIndexF);
  Serial.println(F("°F"));*/
  
}

void GetSoilMoistureSensorData(){

  //Do this for both soil moisture sensors and pairs of LEDs
  for(int i = 0; i < SOIL_SENSOR_AMT; i++){

    int moistureValue = analogRead(SOIL_SENSOR_PINS[i]);
    Serial.println(moistureValue);
  
    //If there is a difference
    if (abs(validSoilSensorReadings[i] - moistureValue) > 10) {
      validSoilSensorReadings[i] = moistureValue;
    }
    
    soilSensorResults[i] = map(validSoilSensorReadings[i], wetProbe, dryProbe, 0, 4);  // scale analog input to a smaller range for wet to dry
  
    /*Serial.print(F("Sensor  "));
    Serial.print(i);
    Serial.print(F(": "));
    DisplaySoilSensorResult(soilSensorResults[i]);*/

  }
  
}

/*void DisplaySoilSensorResult(int result){
  
  // display the correct soil moisture level on the display
  // lower voltages represent more wet levels
  switch (result) {
    case 0:
      Serial.println(F("Wet"));
      break;
    case 1:
      Serial.println(F("Damp"));
      break;
    case 2:
      Serial.println(F("Moist"));
      break;
    case 3:
      Serial.println(F("Dry"));
      break;
    case 4:    // same as case 3, due to how map works.
      Serial.println(F("Dry"));
      break;
  }

}*/

void DisplaySoilSensorResultOnScreen(int result){
  
  // display the correct soil moisture level on the display
  // lower voltages represent more wet levels
  switch (result) {
    case 0:
      tft.println(F("Wet "));
      break;
    case 1:
      tft.println(F("Damp"));
      break;
    case 2:
      tft.println(F("Moist"));
      break;
    case 3:
      tft.println(F("Dry "));
      break;
    case 4:    // same as case 3, due to how map works.
      tft.println(F("Dry "));
      break;
  }

}
