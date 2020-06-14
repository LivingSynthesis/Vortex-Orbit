# The Vortex
This is the Vortex Dream Code, it is the framework for controlling the colors and flash patterns on the Vortex, feel free to use or modify it!

# User Guide

## Basic operation

Your orbit has 3 buttons:

(A_B_____•______Power)

Turn on your orbit by flipping the power button and use A and B to cycle forward or backwards through the modes. That's it!

Your orbit comes with 14 pre-programmed modes.

## Randomizer Mode

The easiest way to change the modes on your orbit is to let the orbit generate new modes for you. The Vortex knows color theory
and will generate a matching color preset and pick a random pattern for you.

To enter randomizer mode, go to a mode you wish to change, and then press and hold A until you see white lights, then release.
You're now in randomizer mode. The orbit will generate a random pattern for you every 8 seconds. Once you get a mode you like, press A to save it into the current mode.
Press B to exit randomizer mode without saving.

## Manualy creating a mode

Each mode is defined by a set of up to 8 colors, and a pattern for those colors.

### Selecting the colors
To modify the colors, navigate to the mode you want to update, then press and hold A until you see yellow lights, then release.
From here, you will see the current colors displayed on the orbit. 
Press A (next) and B (previous) to navigate to the color you want to change, then press and hold A to enter color selection for that color.
From here, press A and B to pick the color range you want, then press and hold A to select.
Next, press A and B to pick exactly the color you want, then press and hold A to select.
Finally, press A and B to pick how saturated you want the color, and press and hold A to select.
After you're done picking the colors, press and hold A to confirm.

At any time you can press and hold B to back out of where you are and cancel your changes.

### Selecting the pattern
To modify the pattern, navigate to the mode you want to update, then press and hold A until you see blue lights then release.
From here, A and B cycle through the different patterns for the mode.
Press and hold A to confirm selection, or press and hold B to cancel.

## Updating modes from your computer

If you want to modify themodes of your vortex with a keyboard and mouse, you can upload modes to it from your computer.

* Get the Arduino IDE
* Connect your Vortex via USB
* Open Arduino IDE
* Under Tools -> Port, select the vortex - Something like /dev/cu.usbmodem144. If you cannot see the vortex show up under ports when you connect:
  * Make sure you're using a cable that support data transfer. Try a few.
  * Make sure you have the driver (should come with your OS but who knows...). Google for Trinket M0 drivers and install them.
* Open Tools -> Serial Monitor
* Press and Hold Button B (closer to center) on your vortex
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
* Download and install Trinket M0 board drivers: 
 * Add the "additional url" according to https://learn.adafruit.com/add-boards-arduino-v164/setup
 * Then install from Tools -> Boards -> Board Manager -> Adafruit SAMD
* Select the "Trinket M0" from the Tools -> Board
* Select TinyUSB from Tools -> USB Stack
* Select USBTinyISP from Tools -> Programmer
* Install the necessary libraries: Sketch -> Include Library -> Manage Libraries. You will need to add FastLED, Adafruit_DotStar and FlashStorage. Maybe more but it will tell you what's missing in the next step.
* You can now upload the existing code into your vortex! Click the "->" button at the top, or Sketch -> Upload.
