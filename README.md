# Vortex
This is the Vortex Dream Code, it is the framework for controlling the colors and flash patterns on the Vortex, feel free to use or modify it!

# User Guide

If you want to more easily work on the patterns of your vortex, you can upload patterns to it from your computer.

* Get the Arduino IDE
* Connect your Vortex via USB
* Open Arduino IDE
* Under Tools -> Port, select the vortex - Something like /dev/cu.usbmodem144. If you cannot see the vortex show up under ports when you connect:
  * Make sure you're using a cable that support data transfer. Try a few.
  * Make sure you have the driver (should come with your OS but who knows...). Google for Trinket M0 drivers and install them.
* Open Tools -> Serial Monitor
* Press and Hold Button 2 (closer to center) on your vortex
* You will get a list of patterns such as "<0, 9, 4, 192, 255, 170, 110, 255, 170, 160, 255, 170, 230, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0>"
  * The first number is the mode index - the first mode is 0, second one is 1, etc.
  * The second number is the pattern - 0 is ribbon, 1 is tracer, etc.. Try them and see!
  * The 3rd number is number of colors in the pattern
  * The next 24 numbers are the 8 possible colors in HSV triplets from 0 to 255. You can read more details about it at https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors. For example 96, 255, 200 will be a bright green. 96 is green, 255 is how rich the color is, and 200 is how bright it is.
* To modify one of the patterns, paste a different set of numbers into the Serial Monitor box at the top and click send.

# Developer Guide

If you want to modify the code or create new patterns, make sure you can do the steps above. After that, 
* Download the code from this repo
* Open VortexDreamEcho.ino file with your Arduino IDE
* Download and install Trinket M0 board drivers: Tools -> Boards -> Board Manager -> Adafruit SAMD
* Select the "Trinket M0" from the Tools -> Board
* Select TinyUSB from Tools -> USB Stack
* Select USBTinyISP from Tools -> Programmer
* Install the necessary libraries: Sketch -> Include Library -> Manage Libraries. You will need to add FastLED, Adafruit_DotStar and FlashStorage. Maybe more but it will tell you what's missing in the next step.
* You can now upload the existing code into your vortex! Click the "->" button at the top, or Sketch -> Upload.
