# Improved Soil Moisture Sensor[v1.0]

This project aims at making the soil moisture sensor project more practical (not necessarily cheaper) than the original that has been done many times with a few LED indicators. To try and improve it, the LEDs are replaced by a 3.2" touchscreen display that can switch between plants depending on the amount of soil moisture sensors connected. Additionally, it can display the approximate temperature and humidity of the room as well as auto-water your plant by tipping over a small cup of water when attached to the servo. The touchscreen displays the temperature, humidity, and current soil moisture level of the plants, indicated by the 4 status' Wet, Damp, Moist, and Dry and an image of a plant that becomes more or less blue depending on the status. 

Repository Contents
============
* **/Bitmap Images** - Contains the .BMP files that were converted to code and used in the project and .psd (Photoshop CS6) files have been included for easy of modification if you desire to do so
* **/Hardware Diagrams** - .fzz (Fritzing files) and .jpg's that show the hardware and how it was wired
* **/Source Code** - Includes 2 folders, the original project that was built first as a test and the improved project
  * **/Source Code/ImprovedSoilMoistureSensor** - Contains the .cpp and .c files needed to run the improved project
  * **/Source Code/OriginalSoilMoistureSensor** - Contains the .cpp and .c files needed to run the original project

Materials
=========
**Original Project:**
* 1 x Arduino UNO
* 4 x 220ohm Resistors
* 4 LEDs

**Improved Project:**
* 1 x 3.2” TFT LCD with Touchscreen Breakout Board w/microSD socket
* 1 x SG90 Micro Servo
* 1 x Small Plastic Cup
* 1 x Servo Attachment (to tip the cup and not spill water everywhere)
* 1 x Elegoo MEGA 2560 R3 (More pins than the UNO)
* 1 x DHT11 (Temperature & Humidity Sensor)

**Materials In Common:**
* Several Jumper Wires (20 - 40)
* Breadboard
* 2 x Capacitive Soil Moisture Sensors v1.2

Requirements
============

### Original Project:
None yay!

### Improved Project:
**Arduino Libraries:**
* Wire - Standard Library (just include it)
* Servo - Standard Library (just include it)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) 
* [Adafruit TFTLCD](https://github.com/adafruit/TFTLCD-Library)
* [Adafruit Touchscreen](https://github.com/adafruit/Adafruit_TouchScreen)
* [Adafruit DHT](https://github.com/adafruit/DHT-sensor-library)

**Software:**
  * [Img2Code](https://github.com/ehubin/Adafruit-GFX-Library/tree/master/Img2Code) - Adafruit Java Application that allows you to convert from a BMP image to code.
    * Resource on how to use it - [Electronics Lab Tutorial](https://www.electronics-lab.com/project/display-custom-bitmap-graphics-on-an-arduino-touch-screen-and-other-arduino-compatible-displays/)
    * I had some issues running this program (on Windows at least). I had to create a gradle.properties file in the Img2Code folder downloaded from the GitHUb, and point it to my JDK with "org.gradle.java.home=C:\\Program Files\\Java\\jdk1.8.0_191". After this, I ran the ./gradlew build and then ./gradlew run in the cmd prompt in the Img2Code directory and it worked. If you run into problems, the GitHub doesn't tell you any of this though, so if there is any other software for convering from a BMP image to code then don't be afraid to try it.
 

Build Instructions
==================

Usage
=====

Team
=====
Mattthew Hamilton - I'm the only one on the team.

Credits
=======
* Jeff Bishop (A.K.A. Eli The Computer Guy) for inspiring the original project -  https://www.elithecomputerguy.com/2019/07/soil-moisture-sensor-led-alert-with-arduino-uno/
