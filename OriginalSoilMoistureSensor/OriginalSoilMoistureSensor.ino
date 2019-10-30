/*

This program will read the analog moisture level of
2 capactive soil moisture sensors, and output whether 
it's dry, moist, damp, or wet.
 
Some code copied from: 
https://github.com/GadgetReboot/Arduino/blob/master/Uno/SSD1306_Soil_Meter/SSD1306_Soil_Meter.in
Code by GitHub user: GadgetReboot
Date Created: Sept.6, 2018

Modified by: Matthew Hamilton
Date Modified: Oct.29, 2019
*/

int ledGreen1 = 3;
int ledRed1 = 2;
int ledGreen2 = 5;
int ledRed2 = 4;

int soilMoistureSensor1 = A0;
int soilMoistureSensor2 = A1;

const int wetProbe      = 300;   // wet readings are around 1.5v or analog input value ~300
const int dryProbe      = 620;   // dry readings are around   3v or analog input value ~620
int validSensorReading1  = 0;    // valid sensor analog reading to record
int validSensorReading2  = 0;
int sensorResult1;               // scaled sensor data [0..3] = [wet, damp, moist, dry]
int sensorResult2;               // scaled sensor data [0..3] = [wet, damp, moist, dry]

void setup() {

  Serial.begin(9600);
  pinMode(ledGreen1, OUTPUT);
  pinMode(ledRed1, OUTPUT);
  pinMode(ledGreen2, OUTPUT);
  pinMode(ledRed2, OUTPUT);

}

void loop() {
 
  int moistureValue1 = analogRead(soilMoistureSensor1);
  int moistureValue2 = analogRead(soilMoistureSensor2);

  Serial.println(moistureValue1);
  Serial.println(moistureValue2);

  //If there is a difference
  if (abs(validSensorReading1 - moistureValue1) > 10) {
    validSensorReading1 = moistureValue1;
  }

  if (abs(validSensorReading2 - moistureValue2) > 10) {
    validSensorReading2 = moistureValue2;
  }

  sensorResult1 = map(validSensorReading1, wetProbe, dryProbe, 0, 4);  // scale analog input to a smaller range for wet to dry
  sensorResult2 = map(validSensorReading2, wetProbe, dryProbe, 0, 4);

  //Output values to Serial Log
  Serial.print("Sensor 1: ");
  DisplaySensorResult(sensorResult1);
  Serial.print("Sensor 2: ");
  DisplaySensorResult(sensorResult2);

  //Change LED colours
  if(sensorResult1 >= 2){

    digitalWrite(ledGreen1, LOW);
    digitalWrite(ledRed1, HIGH);
    
  }else{

    digitalWrite(ledGreen1, HIGH);
    digitalWrite(ledRed1, LOW);
    
  }

   if(sensorResult2 >= 2){

    digitalWrite(ledGreen2, LOW);
    digitalWrite(ledRed2, HIGH);
    
  }else{

    digitalWrite(ledGreen2, HIGH);
    digitalWrite(ledRed2, LOW);
    
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
