/*

Project Summary:
This program will read the analog moisture level of
2 capactive soil moisture sensors, and output whether 
it's dry, moist, damp, or wet.

Project Idea & Components From: 
Jeff Bishop (Eli The Computer Guy)

Parts:
- 4 x LEDs (2 green, 2 red)
- 4 x 220ohm resistors
- 2 x Soil moisture sensors

Connections: 
- Green LEDs on pins 3 and 5
- Red LEDs on pins 2 and 4
- Soil moisture sensors on pins A0 and A1

Some code copied from: 
https://github.com/GadgetReboot/Arduino/blob/master/Uno/SSD1306_Soil_Meter/SSD1306_Soil_Meter.ino
Code by GitHub user: GadgetReboot
Date Created: Sept.6, 2018

Modified by: Matthew Hamilton
Date Modified: Oct.29, 2019
*/

int const SENSOR_AMT = 2;

int greenLEDS[] = {3, 5};
int redLEDS[] = {2, 4};
int moistureSensorPins[] = {A0, A1}; // soil moisture sensor pins
int validSensorReadings[] = {0, 0};   // valid sensor analog reading to record
int sensorResults[] = {0, 0};         // scaled sensor data [0..3] = [wet, damp, moist, dry]

const int wetProbe      = 300;   // wet readings are around 1.5v or analog input value ~300
const int dryProbe      = 620;   // dry readings are around 3v or analog input value ~620

void setup() {

  Serial.begin(9600);

  for(int i = 0; i < SENSOR_AMT; i++){
  
    pinMode(greenLEDS[i], OUTPUT);
    pinMode(redLEDS[i], OUTPUT);

  }

}

void loop() {

  //Do this for both soil moisture sensors and pairs of LEDs
  for(int i = 0; i < SENSOR_AMT; i++){

    int moistureValue = analogRead(moistureSensorPins[i]);
    Serial.println(moistureValue);
  
    //If there is a difference
    if (abs(validSensorReadings[i] - moistureValue) > 10) {
      validSensorReadings[i] = moistureValue;
    }
    
    sensorResults[i] = map(validSensorReadings[i], wetProbe, dryProbe, 0, 4);  // scale analog input to a smaller range for wet to dry
  
    //Output values to Serial Log
    Serial.print("Sensor  ");
    Serial.print(i);
    Serial.print(": ");
    DisplaySensorResult(sensorResults[i]);
  
    //Change LED colours
    if(sensorResults[i] >= 2){
  
      digitalWrite(greenLEDS[i], LOW);
      digitalWrite(redLEDS[i], HIGH);
      
    }else{
  
      digitalWrite(greenLEDS[i], HIGH);
      digitalWrite(redLEDS[i], LOW);
      
    }

  }
  
  delay(500);

}

void DisplaySensorResult(int result){
  
  // display the correct soil moisture level on the display
  // lower voltages represent more wet levels
  switch (result) {
    case 0:
      Serial.println("Wet");
      break;
    case 1:
      Serial.println("Damp");
      break;
    case 2:
      Serial.println("Moist");
      break;
    case 3:
      Serial.println("Dry");
      break;
    case 4:    // same as case 3, due to how map works.
      Serial.println("Dry");
      break;
  }

}
