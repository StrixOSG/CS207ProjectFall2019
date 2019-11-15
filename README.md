# Improved Soil Moisture Sensor [v1.0]

<p align="center">
  <a href="#repository-contents">Repository Contents</a> •
  <a href="#materials">Materials</a> •
  <a href="#requirements">Requirements</a> •
  <a href="#build-instructions">Build Instructions</a> •
  <a href="#usage">Usage</a> •
  <a href="#team">Team</a> •
  <a href="#credits">Credits</a> •
</p>

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
* 1 x [Arduino UNO R3](https://store.arduino.cc/usa/arduino-uno-rev3)
* 4 x 220ohm Resistors
* 4 x LEDs (2 Green and 2 Red, or some way to differentiate Wet or Dry)

**Improved Project:**
* 1 x [3.2” TFT LCD with Touchscreen Breakout Board w/microSD socket](https://www.adafruit.com/product/1743)
* 1 x SG90 Micro Servo
* 1 x Small Plastic Cup (I just used a smaller than normal red solo cup)
* 1 x Servo Attachment (To tip the cup and not spill water everywhere)
* 1 x [Elegoo MEGA 2560 R3](https://www.amazon.ca/gp/product/B01H4ZLZLQ) (Note: An Arduino instead of an Elegoo will work, it's just more expensive)
* 1 x DHT11 (Temperature & Humidity Sensor)

**Materials In Common:**
* Several Jumper Wires (20 - 40)
* Breadboard
* 2 x [Capacitive Soil Moisture Sensors v1.2](https://www.amazon.ca/gp/product/B07H3P1NRM/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)
* Several Male Header Pins

I bought a [ELEGOO UNO Project Super Starter Kit](https://www.amazon.ca/Elegoo-Project-Starter-Tutorial-Arduino/dp/B01D8KOZF4), which contained a lot of the components such as the Jumper Wires, Breadboard, Header Pins, DHT11, SG90 Micro Servo, Resistors, UNO R3, and LEDs.

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
### Original Project:
For this project, you can use an UNO or a Mega. Fair warning that this project does not translate very well to the improved version. The point of setting this project up is to test the Mega, and the Soil Moisture Sensors and establish some simpler code to work off of.

#### Wiring:
* Wire the negative legs of the green LEDs to digital pins 3 and 5, and the red LEDs to digital pins 2 and 4. 
* Having some resistors (220ohm) connected to each of the positive legs of the LEDs is a good idea, just so they don't burn out as fast. The other side of the resistor should go to ground.
* The AOUT (Analog Out) pins on the soil moisture sensors should be connected to pins A0 and A1. Then the VCC's to 5V, and GND's to the GND.
* More details including diagrams (except for the above specified digital and analog pins to connect to) can be found [here](https://www.elithecomputerguy.com/2019/07/soil-moisture-sensor-led-alert-with-arduino-uno/), where the project was originally obtained from. Code for the idea of splitting the soil moisture values into 4 (Dry, Damp, Moist, and Dry) can be found [here](https://github.com/GadgetReboot/Arduino/blob/master/Uno/SSD1306_Soil_Meter/SSD1306_Soil_Meter.ino)

### Improved Project:
Now we get into the improvements. Like I mentioned in the Original Project's build instructions, almost everything changes except that we use the Mega and Soil Moisture Sensors.

#### Part Setup & Prep Work:
* The first step, is to solder on some male header pins to the 8-bit side of the 3.2" TFT Touchscreen. Be careful that it's not the SPI (Serial Peripheral Interface) side. [Adafruit](https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/pinouts) has great detailed information about the differences between the two, but essentially it comes down to speed and how many pins you have. Since we have the Mega, we don't have to worry about using more pins so 8-bit mode is what is used for this project. SPI uses less pins, but it's 2 - 4 times slower than 8-bit mode at updating the touchscreen display.
  * Note: One problem encountered was that the pins weren't soldered well enough, which caused some issues when testing if the touchscreen was connected and working.

<p align="center">
 <img src="https://github.com/StrixOSG/CS207ProjectFall2019/blob/master/README%20Images/Soldered%208-bit%20Side.JPG" width="480">
</p>

* Next, the servo needs 3 male header pins.
* Lastly, attach a servo attachment to a red solo cup. I suggest zip ties and glue to ensure that the attachment stays in-place, so it remains securely attached to the cup while being tipped.

<p align="center">
 <img src="https://github.com/StrixOSG/CS207ProjectFall2019/blob/master/README%20Images/Cup%20With%20Servo%20Attachment.jpg" width="320">
</p>

#### Wiring:
* There isn't anything special with the wiring, except where the pins go. Just in case the Fritzing diagrams below aren't clear enough to follow here's the pins:

* Touchscreen (8-bit mode):
    * D0,XP to digital pin 22
    * D1,YM to digital pin 23
    * D2 to digital pin 24
    * D3 to digital pin 25
    * D4 to digital pin 26
    * D5 to digital pin 27
    * D6 to digital pin 28
    * D7 to digital pin 29
    * C/D,YP to analog pin 3
    * CS,XM to analog pin 2
    * WR to analog pin 1
    * RD to analog pin 0
    * RST to reset pin
    
    * Soil Moisture Sensors to anlog pins 6 and 7
    * Temp & Humidity Sensor to digital pin 2
    * Servo to digital pin 3

* WARNING: Adafruit's tutorial states that the RST Pin can be A4 or another pin, but in my case this caused the touchscreen to not be read properly, so I connected it to the reset pin on the Mega.
* The XP, YM, YP, XM are the touchscreen's resistive sensor pins (detects touch) and they are to be connected to D0, D1, C/D, and CS respectively, so they all go to the same pins, 22, 23, A3, and A2 respectively.
* The soil moisture sensor, we setup in the original project, it is the same except which analog pins it connects to. 
* The servo's red wire goes to 5v, and black to GND. 
* The DHT11 (temp & humidity) sensor can be either 4 or 3 pins, if it does have 4 (unlike the one used in the project) then ignore one of the pins (search for your sensors pin layout to determine which pin this is). On the 3 pin sensor for this project, the ground is the right pin, middle is the data pin, and the left is the ground.

**Breadboard Diagram**

<p align="center">
 <img src="https://github.com/StrixOSG/CS207ProjectFall2019/blob/master/Hardware%20Diagrams/ImprovedSoilMoistureSensorBreadboard.jpg" width="480">
</p>

**Schematic Diagram**

<p align="center">
 <img src="https://github.com/StrixOSG/CS207ProjectFall2019/blob/master/Hardware%20Diagrams/ImprovedSoilMoistureSensorSchematic.jpg" width="480">
</p>

Usage
=====
First step is to ensure that everything is wired and built properly.

### Original Project:
 1. Download the [original soil moisture sensor project folder](https://github.com/mattiasjahnke/arduino-projects).
 2. Open it in Arduino and run it on the device.
 3. Place soil moisture sensors in plants. Or grasp one of the soil moisture sensors in your hand and eventually the green LED should turn on, because your hand is damp or moist instead of dry when its not in anything.

### Improved Project:
 1. Download the [improved soil moisture sensor project folder](https://github.com/mattiasjahnke/arduino-projects).
 2. Open it in Arduino and run it on the device.
 3. If the servo is connected to the cup then it should immediately start tipping it back and forth, if the soil moisture sensors are dry. To prevent this, either hold them or place them in the plants non-dry soil or a cup of water (or disconnect the servo).
 4. Temperature, Humidity, and the visual state (partially or fully blue plant image) and text state (Wet, Damp, Moist, Dry) of plant  1's soil moisture should be displayed.
 5. Tap the next button to switch to plant 2 (the other soil moisture sensor), and it's state should be displayed.
 6. The next button should disappear, and previous button should be on the other side of the screen.
 7. Switch back and forth and monitor your plants water levels, and the temperature and humidity of the room :)

Team
=====
Mattthew Hamilton - I'm the only one on the team.

Credits
=======
* Jeff Bishop (A.K.A. Eli The Computer Guy) for inspiring the original project -  https://www.elithecomputerguy.com/2019/07/soil-moisture-sensor-led-alert-with-arduino-uno/
