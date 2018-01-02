# NeopixelMarlinSlave
Arduino sketch for utilizing second arduino with i2c to drive neopixel strips for Marlin firmware

# Warning disclaimer

You are on grounds of the test area, everything here you use at your own risk, and probably shouldn't try on production machines without proper knowledge.

# Why like this?

Because other solutions didn't satisfy my needs:) Neopixel support is built in Marlin firmware, however Neopixels require precise timings and to achieve that interrupts must be disabled during updating the strips. Because of that we can't use any animations during printing (steppers need interrupts). 
Also neopixels depending on the strip length can use a lot of resources, and this offloads the main CPU. 

# OK, how to install?

First you need additional arduino board. You can utilize whatever board you want depending on what you have and how many pixels you need (before rewiring your printer I suggest to compile the script with PixelCount set to your number of LEDs, to check if the memory is sufficient). 
Then you need to hook up your I2C wiring, to check which pins you need to wire check https://www.arduino.cc/en/Reference/Wire or other reference to your board. 
Remember to connect GND between the boards, and remember to use level shifter if your main board opperates at different logic levels than slave board (e.g. Arduino Due@3.3V vs Arduino Mega@5V).
Neopixels connect according to schematics from adafruit.

For my sketch you will need Adafruit_NeoPixel and NeoPixelBus_by_Makuna library, both available with Arduino IDE library manager. 

# TO-DO list:

1. TEST
2. Wrap up the script in class
3. Add handling of more strips at once //My vision is that this could evolve to something like complete light manager for our printers. On separate chip we have more possibilities to easily add stuff like adding IR sensor etc. 
4. Get support and integration in Marlin firmware (that's probably most important point for now).
5. Make some more fancy animations, we have plenty of CPU power to utilize:) 

# How can You help?

Hey, it's github. Fell free to Fork->Commit->Make pull request :) I'm new to github, so I'll probably be lousy maintainer, but feel free to drop me a message with any suggestion that comes to your mind. 

# All kudos to Marlin Firmware team!
