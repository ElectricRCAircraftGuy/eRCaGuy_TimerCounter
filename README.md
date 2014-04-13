Readme last updated: 12 April 2014

**Instructions**
Open the file called "Timer2_Counter_Basic_Example.ino," in the Arduino IDE.  Upload the code to your Arduino.  Open the Serial Monitor to view the output.  Read the code and comments, and the links below, for more info.

**Basic Summary**
"I wrote a "libary" to get 0.5us precision on a "micros()" replacement function, so that I can get repeatable results reading a PWM or PPM signal, to within 1us.  I searched all around the internet and could not find something comparable (or that was easy to use, and maintained the Arduino's ability to write PWM signals via the Servo Libary), so I think this is my first real contribution to the world of Arduino and Radio Control."  

**Description**
This Timer2_Counter code is a very generic timer tool to be used in Arduino boards in conjunction with, or in replacement of the built-in Arduino micros() function.  I decided to write this code because I needed a really precise timer to be able to measure Radio Control pulse width signals using external interrupts and pin change interrupts, and the built-in Arduino micros() function only has 4 microsecond precision, which allows for a lot of variability, or "noise" in the readings.  To avoid this variability, while keeping the Atmel 16-bit Timer1 free to continue powering the Servo library, I wrote this code to utilize the 8-bit Timer2.  This created a significant challenge, however, in carefully counting timer overflows while ensuring that no overflow count is missed.  Through some careful coding I now have it functioning perfectly, counting all overflow interrupts.  It works well.

**For more information on this code see here (http://electricrcaircraftguy.blogspot.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html) and here (http://www.instructables.com/id/How-to-get-an-Arduino-micros-function-with-05us-pr/).**

Be sure to check out the links above.  I hope you find this useful.
~Gabriel Staples
http://electricrcaircraftguy.blogspot.com/