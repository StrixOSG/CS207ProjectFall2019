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
    D1,YM to digital pin 23
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

  Bitmap image drawing code
  https://www.electronics-lab.com/project/display-custom-bitmap-graphics-on-an-arduino-touch-screen-and-other-arduino-compatible-displays/
  Code by: Nick Koumaris

  Button code
  https://learn.adafruit.com/arduin-o-phone-arduino-powered-diy-cellphone/arduin-o-phone-sketch
  Code by: Adafruit

Modified by: Matthew Hamilton
Date Modified: Nov.10, 2019
*/

/////////////////////   BEGIN TOUCHSCREEN   /////////////////////////////////////////
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

//BMP Images
extern uint8_t soilMoistureMeterBackground[];
extern uint8_t soilMoistureMeterWet[];
extern uint8_t soilMoistureMeterMoist[];
extern uint8_t soilMoistureMeterDamp[];
extern uint8_t soilMoistureMeterDry[];

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

#define MINPRESSURE 10
#define MAXPRESSURE 1000

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

#define PLANT_GREEN 0x0DE9
#define WATER 0x35FF
#define VASE_BACKGROUND 0xBBC1

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40

#define TOTAL_BUTTONS 6
char buttonLabels[TOTAL_BUTTONS][15] = {"Next","Previous","Very Low","Low","Moderate","High"};
uint16_t buttonColors[2] = {WATER, GREEN};
Adafruit_GFX_Button buttons[TOTAL_BUTTONS];

/////////////////////   END TOUCHSCREEN   ///////////////////////////////////////////

/////////////////////   BEGIN SOIL MOISTURE SENSORS   ///////////////////////////////

const int SOIL_SENSOR_PINS[] = {A6, A7};  // soil moisture sensor pins
const int MAX_PLANTS = 2;
int validSoilSensorReadings[] = {0, 0};           // valid sensor analog reading to record
int soilSensorResults[] = {0, 0};                 // scaled sensor data [0..3] = [wet, damp, moist, dry]
int previousSoilSensorResult = -1;
int currentPlant = 0;

//Set the default plant moisture setting to Moderate (3)
int plantMoistureSetting[MAX_PLANTS] = {2, 2};

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
int angle = 0;
int totalWateringTime = 5000;
unsigned long wateringStartTime = 0;
bool wateringInProgress = false;

Servo servo;

/////////////////////    END SERVO   /////////////////////////////////////////////

void setup() {
  
  Serial.begin(9600);

  SetupTouchscreen();

  //Setup servo
  servo.attach(SERVO_PIN);

  //Setup temp & humidity sensor
  dht.begin();
  
}

void SetupTouchscreen(){

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
  tft.fillScreen(WHITE);

  CreateButtons();
  
}

void loop() {
    
  GetSoilMoistureSensorData();
  GetTHSensorData();
  DisplayDataOnTouchScreen();
  DetectTouch();
  AutoWaterWithServo();

}

void CreateButtons(){
  
  if(currentPlant < (MAX_PLANTS - 1)){
    
    //Next plant
    //initButton(Touchscreen, X, Y, Width, Height, outline, fill, text color, text, text size)
    buttons[0].initButton(&tft, 175, 300, 100, 30, WHITE, buttonColors[0], WHITE, buttonLabels[0], 1);
    buttons[0].drawButton();

  }

  if(currentPlant > 0){
    
    //Previous plant
    buttons[1].initButton(&tft, 75, 300, 100, 30, WHITE, buttonColors[0], WHITE, buttonLabels[1], 1);
    buttons[1].drawButton();

  }

  //Plant Categories
  
  //Very Low Moisture
  if(plantMoistureSetting[currentPlant] == 0){
    buttons[2].initButton(&tft, 50, 100, 75, 30, WHITE, buttonColors[1], WHITE, buttonLabels[2], 1);
  }else{
    buttons[2].initButton(&tft, 50, 100, 75, 30, WHITE, buttonColors[0], WHITE, buttonLabels[2], 1);
  }
  buttons[2].drawButton();
  
  //Low Moisture
  if(plantMoistureSetting[currentPlant] == 1){
    buttons[3].initButton(&tft, 50, 135, 75, 30, WHITE, buttonColors[1], WHITE, buttonLabels[3], 1);
  }else{
    buttons[3].initButton(&tft, 50, 135, 75, 30, WHITE, buttonColors[0], WHITE, buttonLabels[3], 1);
  }
  buttons[3].drawButton();
  
  //Moderate Moisture
  if(plantMoistureSetting[currentPlant] == 2){
    buttons[4].initButton(&tft, 50, 170, 75, 30, WHITE, buttonColors[1], WHITE, buttonLabels[4], 1);
  }else{
    buttons[4].initButton(&tft, 50, 170, 75, 30, WHITE, buttonColors[0], WHITE, buttonLabels[4], 1);
  }
  buttons[4].drawButton();
  
  //High Moisture
  if(plantMoistureSetting[currentPlant] == 3){
    buttons[5].initButton(&tft, 50, 205, 75, 30, WHITE, buttonColors[1], WHITE, buttonLabels[5], 1);
  }else{
    buttons[5].initButton(&tft, 50, 205, 75, 30, WHITE, buttonColors[0], WHITE, buttonLabels[5], 1);
  }
  buttons[5].drawButton();
  
}

void DetectTouch(){

  TSPoint point = ts.getPoint();
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  //If touch has been detected
  if(point.z > MINPRESSURE && point.z < MAXPRESSURE){

      // scale from 0->1023 to tft.width
      point.x = map(point.x, TS_MINX, TS_MAXX, 0, tft.width());
      point.y = map(point.y, TS_MINY, TS_MAXY, 0, tft.height());

      Serial.print("X: ");
      Serial.println(point.x);
      Serial.print("Y: ");
      Serial.println(point.y);
      
    //Detect if button pressed
    for (int i = 0; i < TOTAL_BUTTONS; i++) {
        
        if(buttons[i].contains(point.x, point.y)) {

           HandleButtons(i);
          
        }

      }
      
  }

}

void HandleButtons(int buttonNumber){

  switch(buttonNumber){

    case 0: //Next Plant
      if(currentPlant < (MAX_PLANTS - 1)){
    
        currentPlant++;
        Serial.println(currentPlant);
    
      }
    break;
   case 1:  //Previous plant
      if(currentPlant > 0){
      
        currentPlant--;
        Serial.println(currentPlant);
        
      }
      break;
    case 2: //Very Low Moisture
      plantMoistureSetting[currentPlant] = 0;
      break;
    case 3: //Low Moisture
      plantMoistureSetting[currentPlant] = 1;
      break;
    case 4: //Moderate Moisture
      plantMoistureSetting[currentPlant] = 2;
      break;
    case 5: //Moderate Moisture
      plantMoistureSetting[currentPlant] = 3;
      break;
      
  }

  //Some button was tapped so wipe the screen and redraw the buttons
  ResetTouchScreen();
  CreateButtons();
  
}

void DisplayDataOnTouchScreen(){

  tft.setCursor(75, 235);
  tft.setTextColor(WATER, WHITE);
  tft.setTextSize(2);
  DisplaySoilMoistureData();
  
  tft.setCursor(20, 20);
  tft.setTextColor(PLANT_GREEN, WHITE);
  DisplayHumidityData();
  tft.setCursor(20, 40);
  DisplayTempData();
  
}

void DisplaySoilMoistureData(){

  tft.print(F("Plant "));
  tft.print(currentPlant + 1);
  tft.print(F(": "));

  if(previousSoilSensorResult != soilSensorResults[currentPlant]){
    
    drawBitmap(75, 75, soilMoistureMeterBackground, 150, 150, VASE_BACKGROUND);
    previousSoilSensorResult = soilSensorResults[currentPlant];
    DisplaySoilSensorResultOnScreen(soilSensorResults[currentPlant]);

  }
  
}

void ResetTouchScreen(){

  tft.fillScreen(WHITE);
  previousSoilSensorResult = -1;
  
}

void DisplayTempData(){

  tft.print("Temp:  ");
  DisplayCelcius();
  //DisplayFahrenheit();
  
}

void DisplayCelcius(){

  tft.print(celcius);
  tft.print((char)247); //Degrees symbol
  tft.println("C");
  
}

void DisplayFahrenheit(){

  tft.print(fahrenheit);
  tft.print((char)247); //Degrees symbol
  tft.println("F");
  
}

void DisplayHumidityData(){

  tft.print("Humid: ");
  tft.print(humidity);
  tft.print("% ");
  
}

void DisplayHeatIndexData(){
  
  tft.print("Heat index: ");
  tft.print(heatIndexC);
  tft.print((char)247); //Degrees symbol
  tft.println("C");
  tft.print(heatIndexF);
  tft.print((char)247); //Degrees symbol
  tft.println("F");
  
}

void AutoWaterWithServo(){
  Serial.println(soilSensorResults[currentPlant]);
  //If Dry or currently watering
  if((soilSensorResults[currentPlant] >= 3) || wateringInProgress){

    //If just started watering
    if(wateringInProgress == false){

      wateringStartTime = millis();
      wateringInProgress = true;

      //Tilt the servo to 60 (tip the cup to water the plant)
      for(angle = 0; angle < 120; angle++)  
      {                                  
        servo.write(angle);               
        delay(15);                   
      } 
      
    }

    //If watering time is up
    if(millis() > (wateringStartTime + totalWateringTime)){
      
        // Reset the servo to 0 (tilt the cup back upright)
        for(angle = 120; angle > 0; angle--)    
        {                                
          servo.write(angle);           
          delay(15);       
        }

        wateringInProgress = false;

    }
    
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
  for(int i = 0; i < MAX_PLANTS; i++){

    int moistureValue = analogRead(SOIL_SENSOR_PINS[i]);
    //Serial.println(moistureValue);
  
    //If there is a difference
    if (abs(validSoilSensorReadings[i] - moistureValue) > 10) {
      validSoilSensorReadings[i] = moistureValue;
    }
    
    soilSensorResults[i] = map(validSoilSensorReadings[i], dryProbe, wetProbe, 0, 100);  // scale analog input to a smaller range for wet to dry
    Serial.print(F("Sensor 1: "));
    Serial.println(soilSensorResults[0]);
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
    if(result > 60 && result <= 100){
      tft.println(F("Wet  "));
      drawBitmap(75, 75, soilMoistureMeterWet, 150, 150, WATER);
    }
    else if(result > 40 && result < 61){
      tft.println(F("Damp "));
      drawBitmap(75, 75, soilMoistureMeterDamp, 150, 150, WATER);
    }
    else if(result > 20 && result < 41){
      tft.println(F("Moist"));
      drawBitmap(75, 75, soilMoistureMeterMoist, 150, 150, WATER);
    }
    else{
      tft.println(F("Dry  "));
      drawBitmap(75, 75, soilMoistureMeterDry, 150, 150, WATER);
    }

}

void drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
    }
  }
}
