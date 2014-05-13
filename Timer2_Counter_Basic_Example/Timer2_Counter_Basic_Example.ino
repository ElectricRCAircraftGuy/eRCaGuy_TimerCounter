/*
Timer2 Counter Basic Example - Demonstrates use of my Timer2_Counter, which is a timer function with 0.5us precision, 
                               rather than 4us precision like the built-in Arduino micros() function has.
By Gabriel Staples
Visit my blog at http://electricrcaircraftguy.blogspot.com/
-My contact info is available by clicking the "Contact Me" tab at the top of my blog.
-Please support my work & contributions by buying something here: https://sites.google.com/site/ercaguystore1/
My original post containing this code can be found here: http://electricrcaircraftguy.blogspot.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html
Written: 8 Feb. 2014
Updated: 9 Feb. 2014
*/

/*
===================================================================================================
  LICENSE & DISCLAIMER
  
  Copyright (C) 2014 Gabriel Staples.  All right reserved.
  
  This code was written entirely at home, during my own personal time, and is neither a product of work nor my employer.
  Furthermore, unless otherwise stated, it is owned entirely by myself.
  
  ------------------------------------------------------------------------------------------------
  License: GNU General Public License Version 3 (GPLv3) - http://www.gnu.org/licenses/gpl.html
  ------------------------------------------------------------------------------------------------

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

  
void setup() {
  //configure Timer2
  setup_T2(); //this MUST be done before the other functions work; Note: since this messes up PWM outputs on pins 11 & 3, 
              //you can always revert Timer2 back to normal by calling unsetup_T2()
  
  //prepare serial
  Serial.begin(115200);  
  
  //Output a header of info:
  Serial.println("Notes:");
  Serial.println("micros() has a precision of 4us");
  Serial.println("get_T2_count() with unsigned long final data type has a final precision of 1us, and is fast");
  Serial.println("get_T2_count() with float final data type has a final precision of 0.5us, and is not quite as fast");
  Serial.println("get_T2_micros() has a precision of 0.5us, and is slower than the above 2 methods, so one of the above 2 methods is preferred");
  Serial.println("==============================================");
}


void loop() {
  //Grab Start Times
  unsigned long t_start1 = micros(); //us; get the current time using the built-in Arduino function micros(), to a precision of 4us
  unsigned long t_start2 = get_T2_count(); //count units of 0.5us each; get my Timer2 count, where each count represents 0.5us; PREFERRED METHOD
  float t_start3 = get_T2_micros(); //us; get the current time using my Timer2; Note: THE METHOD ONE LINE ABOVE IS PREFERRED OVER THIS METHOD
                                    //since using floats is slower than using unsigned longs
  
  //Wait a bit                       
  delayMicroseconds(1000);
  
  //Grab End Times
  unsigned long t_end1 = micros(); //us; using built-in Arduino function that has a precision of 4us
  unsigned long t_end2 = get_T2_count(); //count units of 0.5us each; using my Timer2 count, where each count represents 0.5us
  float t_end3 = get_T2_micros(); //us; using my Timer2 micros, which has a precision of 0.5us
  
  //Calculate elapsed times
  unsigned int t_elapsed1 = t_end1 - t_start1; //us; using micros()
  unsigned int t_elapsed2_ul = (t_end2 - t_start2)/2; //us; to a precision of 1us, due to using unsigned long data type truncation, using Timer2 count
  float t_elapsed2_fl = (t_end2 - t_start2)/2.0; //us; to a precision of 0.5us, due to using float data type for final time difference calc; note that I divide by 2.0, NOT 2
  float t_elapsed3 = t_end3 - t_start3; //us; to a precision of 0.5us
  
  //Display the results
  Serial.println(""); //insert a space
  Serial.print("elapsed time using micros() = ");
  Serial.print(t_elapsed1);
  Serial.println("us");
  Serial.print("elapsed time using get_T2_count() with unsigned long final data type = ");
  Serial.print(t_elapsed2_ul);
  Serial.println("us");
  Serial.print("elapsed time using get_T2_count() with float final data type = ");
  Serial.print(t_elapsed2_fl);
  Serial.println("us");
  Serial.print("elapsed time using get_T2_micros() = ");
  Serial.print(t_elapsed3);
  Serial.println("us");
  
  //Wait a second before repeating
  delay(1000); 
}





