/*
Timer2 Counter
-A timer function with 0.5us precision, rather than 4us precision like the built-in Arduino micros() function has.
By Gabriel Staples
Visit my blog at http://electricrcaircraftguy.blogspot.com/
-My contact info is available by clicking the "Contact Me" tab at the top of my blog.
-Please support my work & contributions by buying something here: https://sites.google.com/site/ercaguystore1/
My original post containing this code can be found here: http://electricrcaircraftguy.blogspot.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html
Written: 7 Dec. 2013
Last Updated: 23 Feb. 2014
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

/*
CODE DESCRIPTION:
BACKGROUND:
-This code uses Timer2 to create a more precise timer than micros().  Micros() updates only every 4us.  However, I want something that will update every 0.5us, and that's 
what this provides. 
-The downside is that it changes the behavior of PWM output (using analogWrite) on Pins 3 & 11.  Use of my Timer2_Counter also interferes with the "tone()" library. 
--Per the documentation on the tone library, it says that it interferes with PWM output on pins 3 & 11 as well--meaning that use of my Timer2_Counter will also prevent
  using the tone() library simultaneously.  See here for details: http://arduino.cc/en/Reference/tone
-The upside is that I have managed to get a precise timer using the 8-bit Timer2, rather than the 16-bit Timer1, so that I can keep the Timer1 unmodified so I can continue
 to use the servo library as desired.  Note here that the servo library relies on the Atmega328 16-bit Timer1.  I have deciphered this by knowing that
 A) using the servo library at all disables PWM output on pins 9 & 10; see here: http://www.arduino.cc/en/Reference/Servo
 and B) PWM on pins 9 & 10 is controlled by Timer1; see here: http://playground.arduino.cc/Main/TimerPWMCheatsheet
-Note: every now and then, but not very often, you will see that the value returned by get_T2_count or get_T2_micros is late by ~4us.  This is because Timer2 is only
 an 8-bit timer, so every 128us the overflow interrupt is called to increment the overflow counter, and stepping into and out of the interrupt takes 4~5us
 according to Nick Gammon. Source: Nick Gammon; "Interrupts" article; "How long does it take to execute an ISR?" section, found here: http://www.gammon.com.au/forum/?id=11488
IMPLEMENTATION:
-To use this code, simply copy this file (I have it named "Timer2_Counter.ino") into your directory (folder) where you are already working on a specific Arduino file.  
Next time you open up your main file you are working on, this code will automatically open up as an additional tab in the Arduino IDE.  
-I have tested this code ONLY on Arduinos using the Atmega328 microcontroller; more specifically, the Arduino Nano.

I heavily reference the 660 pg. Atmega328 datasheet, which can be found here:  http://www.atmel.com/Images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet.pdf


Version History:
(most recent event last; format year-month-day, ex: 20131211 is Dec. 11, 2013):
20131211 - completed first iteration of code
20140223 - minor change to documentation: added note about the Tone library being affected by this code too

============================================================================================================================================================================
Function Definitions
 ___  _   _  _  _   ___  _____  ___  ___   _  _      ___   ___  ___  ___  _  _  ___  _____  ___  ___   _  _  ___ 
| __|| | | || \| | / __||_   _||_ _|/ _ \ | \| |    |   \ | __|| __||_ _|| \| ||_ _||_   _||_ _|/ _ \ | \| |/ __|
| _| | |_| || .` || (__   | |   | || (_) || .` |    | |) || _| | _|  | | | .` | | |   | |   | || (_) || .` |\__ \
|_|   \___/ |_|\_| \___|  |_|  |___|\___/ |_|\_|    |___/ |___||_|  |___||_|\_||___|  |_|  |___|\___/ |_|\_||___/
                                                     
setup_T2();  //This function MUST be called before any of the other Timer2 functions will work.  This function will generally only be called one time in your setup() loop.
             //"setup_T2()" prepares Timer2 and speeds it up to provide greater precision than micros() can give.
get_T2_count();  //gets the counter from Timer 2. Returns the Timer2 counter value as an unsigned long.  Each count represents a time interval of 0.5us.
                 //note that the time returned WILL update even in Interrupt Service Routines (ISRs), so if you call this function in an ISR, and you want the time to be
                 //as close to possible as a certain event that occured which called the ISR you are in, make sure to call "get_T2_count()" first thing when you enter
                 //the ISR.
                 //Also, note that calling "get_T2_count()" is faster than calling "get_T2_micros()," and is therefore the preferable way to measure a time interval.
                 //For example: call "get_T2_count()" at the beginning of some event, then at the end.  Take the difference and divide it by 2 to get the time interval
                 //in microseconds.
get_T2_micros(); //returns the Timer2 microsecond time, with a precision of 0.5us, as a float.  This function is slower than calling "get_T2_count()," and therefore is not
                 //the preferred way of getting time.  It is better to get the time by calling "get_T2_count()" then dividing the value by 2.  By choosing whether or not
                 //you want to call "get_T2_count()" or "get_T2_micros()," you can decide if you need the extra precision of a float, or not, at the cost of having 
                 //slightly slower code.
reset_T2(); //resets the Timer2 counters back to 0.  Very useful if you want to count up from a specific moment in time, or obtain an "elapsed time."
revert_T2_to_normal(); //this function might also be called "unsetup_T2".  It simply returns Timer2 to its normal state that Arduino had it in prior to calling "setup_T2"
unsetup_T2(); //the exact same as "revert_T2_to_normal()"
T2_overflow_interrupt_off(); //turns off the Timer 2 overflow interrrupt so that you no longer interrupt your code every 128us in order to increment your overflow counter. 
                       //This may be desirable when you are no longer referencing the T2 counter or timer and want your main code to run a touch faster, but you don't want
                       //to call unsetup_T2() in order to change all of Timer2's settings back to default.
                       //Since an interrupt takes ~5us to execute, and my Timer2 will overflow every 128us, disabling the Timer2 overflow interrupt will prevent 
                       //you from losing that amount of time (~5us) every 128us.
                       //Source: Nick Gammon; "Interrupts" article; "How long does it take to execute an ISR?" section, found here: http://www.gammon.com.au/forum/?id=11488
                       //Note: If you diable the Timer 2 overflow interrupt but still call get_TC_count() or get_TC_micros() at least every 128us, you will notice no 
                       //difference in the counter, since calling get_TC_count() or get_TC_micros() also checks the interrupt flag and increments the overflow counter 
                       //automatically.  You have to wait > 128us before you see any missed overflow counts.
T2_overflow_interrupt_on(); //turns Timer 2's overflow interrupt back on, so that the overflow counter will start to increment again; see "T2_overflow_interrupt_off()"
                            //explanation for more details.
============================================================================================================================================================================
References:
-cool font, type "small", from: http://www.network-science.de/ascii/

Additional Resources:
-Nick Gammon's "Timers & Counters" article - http://www.gammon.com.au/forum/?id=11504
-Nick Gammon's "Interrupts" article - http://www.gammon.com.au/forum/?id=11488
*/


//Set up Global Variables
//volatile (used in ISRs)
volatile unsigned long T2_overflow_count = 0; //initialize Timer2 overflow counter
volatile unsigned long T2_total_count = 0; //initialize Timer2 total counter
//normal variables
byte tccr2a_save; //initialize; will be used to backup default settings
byte tccr2b_save; //initialize; will be used to backup default settings


//Interrupt Service Routine (ISR) for when Timer2's counter overflows; this will occur every 128us
ISR(TIMER2_OVF_vect) //Timer2's counter has overflowed 
{
  T2_overflow_count++; //increment the timer2 overflow counter
}


//Configure Timer2
void setup_T2()
{
  //backup variables
  tccr2a_save = TCCR2A; //first, backup some values
  tccr2b_save = TCCR2B; //backup some more values
  
  //increase the speed of timer2; see below link, as well as the datasheet pg 158-159.
  TCCR2B = TCCR2B & 0b11111000 | 0x02; //Timer2 is now faster than default; see here for more info: http://playground.arduino.cc/Main/TimerPWMCheatsheet
  //Note: don't forget that when you speed up Timer2 like this you are also affecting any PWM output (using analogWrite) on Pins 3 & 11.  
  //Refer to the link just above, as well as to this source here:  http://www.oreilly.de/catalog/arduinockbkger/Arduino_Kochbuch_englKap_18.pdf

  //Enable Timer2 overflow interrupt; see datasheet pg. 159-160
  TIMSK2 |= 0b00000001; //enable Timer2 overflow interrupt. (by making the right-most bit in TIMSK2 a 1)
  //TIMSK2 &= 0b11111110; //use this code to DISABLE the Timer2 overflow interrupt, if you ever wish to do so later. (see datasheet pg. 159-160)
  
  //set timer2 to "normal" operation mode.  See datasheet pg. 147, 155, & 157-158 (incl. Table 18-8).  
  //-This is important so that the timer2 counter, TCNT2, counts only UP and not also down.
  //-To do this we must make WGM22, WGM21, & WGM20, within TCCR2A & TCCR2B, all have values of 0.
  TCCR2A &= 0b11111100; //set WGM21 & WGM20 to 0 (see datasheet pg. 155).
  TCCR2B &= 0b11110111; //set WGM22 to 0 (see pg. 158).
}  
  

//get total count for Timer2
unsigned long get_T2_count()
{
  noInterrupts(); //prepare for critical section of code
  uint8_t tcnt2_save = TCNT2; //grab the counter value from Timer2
  boolean flag_save = bitRead(TIFR2,0); //grab the timer2 overflow flag value
  if (flag_save) { //if the overflow flag is set
    tcnt2_save = TCNT2; //update variable just saved since the overflow flag could have just tripped between previously saving the TCNT2 value and reading bit 0 of TIFR2.  
                        //If this is the case, TCNT2 might have just changed from 255 to 0, and so we need to grab the new value of TCNT2 to prevent an error of up 
                        //to 127.5us in any time obtained using the T2 counter (ex: T2_micros). (Note: 255 counts / 2 counts/us = 127.5us)
                        //Note: this line of code DID in fact fix the error just described, in which I periodically saw an error of ~127.5us in some values read in
                        //by some PWM read code I wrote.
    T2_overflow_count++; //force the overflow count to increment
    TIFR2 |= 0b00000001; //reset Timer2 overflow flag since we just manually incremented above; see datasheet pg. 160; this prevents execution of Timer2's overflow ISR
  }
  T2_total_count = T2_overflow_count*256 + tcnt2_save; //get total Timer2 count
  interrupts(); //allow interrupts again
  return T2_total_count;
}


//get the time in microseconds, as determined by Timer2; the precision will be 0.5 microseconds instead of the 4 microsecond precision of micros()
float get_T2_micros()
{
  float T2_micros = get_T2_count()/2.0; 
  return T2_micros;
}


//reset Timer2's counters
void reset_T2()
{
  T2_overflow_count = 0; //reset overflow counter
  T2_total_count = 0; //reset total counter
  TCNT2 = 0; //reset Timer2 counter
  TIFR2 |= 0b00000001; //reset Timer2 overflow flag; see datasheet pg. 160; this prevents an immediate execution of Timer2's overflow ISR
}


//undo configuration changes for Timer2
void revert_T2_to_normal()
{
  T2_overflow_interrupt_off(); //turn off Timer2 overflow interrupts
  TCCR2A = tccr2a_save; //restore default settings
  TCCR2B = tccr2b_save; //restore default settings
}


//same as revert_T2_to_normal()
void unsetup_T2()
{
  revert_T2_to_normal();
}


//Turn off the Timer2 Overflow Interrupt
void T2_overflow_interrupt_off()
{
//  TIMSK2 &= 0b11111110; //use this code to DISABLE the Timer2 overflow interrupt; see datasheet pg. 159-160
  TIMSK2 &= ~(_BV(TOIE2)); //alternate code to do the above; see here for use of _BV: http://194.81.104.27/~brian/microprocessor/BVMacro.pdf 
}


//Turn the Timer2 Overflow Interrupt Back On
void T2_overflow_interrupt_on()
{
//  TIMSK2 |= 0b00000001; //enable Timer2 overflow interrupt. (by making the right-most bit in TIMSK2 a 1); see datasheet pg. 159-160
  TIMSK2 |= _BV(TOIE2); //alternate code to do the above; see here for use of _BV: http://194.81.104.27/~brian/microprocessor/BVMacro.pdf 
}



