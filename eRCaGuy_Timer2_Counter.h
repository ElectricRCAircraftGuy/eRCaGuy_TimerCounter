/*
eRCaGuy_Timer2_Counter
Library webpage: http://electricrcaircraftguy.blogspot.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html

**Basic Summary**
"I wrote a libary to get 0.5us precision on a "micros()" replacement function, so that I can get repeatable results reading a PWM or PPM signal, to within 1us.  I searched all around the internet and could not find something comparable (or that was easy to use, and maintained the Arduino's ability to write PWM signals via the Servo Libary), so I think this is my first real contribution to the world of Arduino and Radio Control."  
 
 By Gabriel Staples
 http://electricrcaircraftguy.blogspot.com/
 -My contact info is available by clicking the "Contact Me" tab at the top of my blog.
 Written: 7 Dec. 2013
 Last Updated: 13 May 2014
 
 Version: 1.0 - first release as an actual Library (instead of just an appended file) - 13 May 2014
 
FOR MORE INFORMATION SEE THE .CPP FILE.

Additional References:
GS: *************NOTE TO SELF************: I referred to the TimerOne Library (http://playground.arduino.cc/Code/Timer1) in order to figure
    out how to make ISR() work within a library. Ultimately, I ended up using the library object pre-instantiation method so that I could
	call a specific object method within the library itself.  See the .cpp file, and the ISR() therein, for details.
*/

/*
===================================================================================================
  LICENSE & DISCLAIMER
  Copyright (C) 2015 Gabriel Staples.  All right reserved.
  
  ------------------------------------------------------------------------------------------------
  License: GNU Lesser General Public License Version 3 (LGPLv3) or later - https://www.gnu.org/licenses/lgpl.html
  ------------------------------------------------------------------------------------------------

  This file is part of eRCaGuy_Timer2_Counter.
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/
===================================================================================================
*/

#ifndef eRCaGuy_Timer2_Counter_h
#define eRCaGuy_Timer2_Counter_h

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

//this has to be declared in order to make it "friend" below
extern "C" void TIMER2_OVF_vect();

class eRCaGuy_Timer2_Counter
{
  public:
    //declare class constructor method
    eRCaGuy_Timer2_Counter();
	//declare other public class methods (functions)
	
	//FOR A FULL DESCRIPTION OF WHAT EACH METHOD BELOW DOES, REFER TO THE .CPP FILE.
	
	void setup(); 
	unsigned long get_count();
	float get_micros();
	void reset();
	void revert_to_normal();
	void unsetup();
	void overflow_interrupt_off();
	void overflow_interrupt_on();
	
	//public variables:
	//N/A
	
  private:
	//declare private class methods (member functions)
	//N/A
    //Declare private variables (ie: "member variables," or variables which are members of and accessible by this class only):
	//volatile (used in ISRs)
	volatile static unsigned long _overflow_count; //Timer2 overflow counter; updated in ISR, so must be declared volatile (see here: http://arduino.cc/en/Reference/Volatile)
	friend void TIMER2_OVF_vect(); //let the ISR access _overflow_count, despite it being a private member
	  //GS: for more info on static class members, see here: http://www.tutorialspoint.com/cplusplus/cpp_static_members.htm (or PDF pg. 148-149)
	unsigned long _total_count; //Timer2 total counter
	byte _tccr2a_save; //will be used to backup default settings
	byte _tccr2b_save; //will be used to backup default settings
};

//Declare the external existence (defined in the .cpp file) of object timer2, so that you can access it in your Arduino sketch simply by including this library, via its header file (ex: #include <eRCaGuy_Timer2_Counter.h>)
//This is absolutely necessary or else the Arduino sketch that includes this library will not compile.
//For more info on "extern" see here: http://www.geeksforgeeks.org/understanding-extern-keyword-in-c/
extern eRCaGuy_Timer2_Counter timer2;
#endif



