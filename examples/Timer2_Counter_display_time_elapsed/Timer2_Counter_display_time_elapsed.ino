/*
Timer2_Counter_display_time_elapsed.ino
Timer2 Counter Basic Example - Demonstrates use of my Timer2_Counter, which is a timer function with 0.5us precision, 
                               rather than 4us precision like the built-in Arduino micros() function has.
By Gabriel Staples
Visit my blog at http://electricrcaircraftguy.blogspot.com/
-My contact info is available by clicking the "Contact Me" tab at the top of my blog.
-Please support my work & contributions by buying something here: https://sites.google.com/site/ercaguystore1/
My original post containing this code can be found here: http://electricrcaircraftguy.blogspot.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html
Written: 17 May 2014
Updated: 17 May 2014
*/

/*
===================================================================================================
  LICENSE & DISCLAIMER
  Copyright (C) 2014 Gabriel Staples.  All right reserved.
  
  ------------------------------------------------------------------------------------------------
  License: GNU General Public License Version 3 (GPLv3) - https://www.gnu.org/licenses/gpl.html
  ------------------------------------------------------------------------------------------------

  This file is part of eRCaGuy_Timer2_Counter.
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/
===================================================================================================
*/

//CODE DESCRIPTION:
//This code demonstrates the use of my Timer2, which provides a more precise timer than micros().  
//micros() has a precision of only 4us.  However, Timer2 keeps track of time to a precision of 0.5us.
//This is especially important in my code which reads an RC receiver PWM signal, which varies from 900~2100us. 
//Though this code demonstrates the use of the Timer_2 functions I have written, it does not adequately demonstrate the 
//real utility of the code, so I will state the following:
//By using my Timer2 timer to measure the PWM high time interval on an RC receiver, in place of using micros(), I can get repeatable 
//pulse width reads with a fluctuation of ~1us, rather than having a read-in range fluctuating by as much as +/- 4~8 us when I use micros().
//This is an increase in precision of ~8x.

//include the library
#include <eRCaGuy_Timer2_Counter.h>

//Note: an object of this class was already pre-instantiated in the .cpp file of this library, so you can simply access its methods (functions)
//      directly now through the object name "timer2"
//eRCaGuy_Timer2_Counter timer2;  //this is what the pre-instantiation line from the .cpp file looks like
  
void setup() {
  //configure Timer2
  timer2.setup_T2(); //this MUST be done before the other Timer2_Counter functions work; Note: since this messes up PWM outputs on pins 3 & 11, as well as 
                     //interferes with the tone() library (http://arduino.cc/en/reference/tone), you can always revert Timer2 back to normal by calling 
                     //timer2.unsetup_T2()
  
  //prepare serial
  Serial.begin(115200);  
  
  //Output a header of info:
  Serial.println(F("Notes:"));
  Serial.println(F("micros() has a precision of 4us"));
  Serial.println(F("get_T2_count() with unsigned long final data type has a final precision of 1us, and is fast"));
  Serial.println(F("get_T2_count() with float final data type has a final precision of 0.5us, and is not quite as fast"));
  Serial.println(F("get_T2_micros() has a precision of 0.5us, and is slower than the above 2 methods, so one of the above 2 methods is preferred"));
  Serial.println(F("=============================================="));
}


void loop() {
  //declare local variables
  static unsigned long t_start = timer2.get_T2_count(); //units of 0.5us; the count accumulated by Timer2_Counter
  
  //acquire time stamps
  unsigned long t_micros = micros();
  unsigned long t_T2_count = timer2.get_T2_count(); //units of 0.5us; the count accumulated by Timer2_Counter
  float t_T2_micros = timer2.get_T2_micros(); //us; the time value accumulated by Timer2_Counter (this is the exact same as doing timer2.get_T2_count()/2.0;)
  
  //See if 1.000003 seconds has elapsed.  If so, print out the time stamps. Note: I am using this elapsed time because I want it to NOT be divisible by 4, so that 
  //you can hopefully see the extra precision provided by the Timer2_Counter library, which the default Arduino micros() function does not have
  if ((t_T2_count - t_start)/2 >= 1000003) //if 1.000003 seconds has elapsed
  {
    t_start = t_T2_count; //update start time
    
    //Print times using micros(), get_T2_count(), and get_T2_micros()
    //Demonstrate several ways of using these functions
    Serial.print("micros() = "); Serial.print(t_micros); Serial.println(" us");
    Serial.print("get_T2_count() = "); Serial.print(t_T2_count); Serial.println(" (units of 0.5 us per count)");
    Serial.print("get_T2_count()/2 = "); Serial.print(t_T2_count/2); Serial.println(" us");
    Serial.print("get_T2_count()/2.0 = "); Serial.print(t_T2_count/2.0); Serial.println(" us");
    Serial.print("get_T2_micros() = "); Serial.print(t_T2_micros); Serial.println(" us");
    Serial.println(""); //add an extra line space
  }
} //end of loop()





