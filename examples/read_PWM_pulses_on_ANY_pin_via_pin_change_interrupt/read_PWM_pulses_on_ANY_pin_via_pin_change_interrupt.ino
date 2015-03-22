/*
pulse_reader_w_pin_change_interrupt_singleCh.ino
-read in any pulsing signal on Arduino pin INPUT_PIN (defined below), to get its period (us) & freq (Hz), with a time resolution of 0.5us
--you can read in any pulse, including standard PWM signals, Radio Control (RC) PWM signals, etc. 
-I am using some low-level AVR code, which requires using some built-in Arduino macros to do pin-mapping. 
-this code only reads in a single channel at a time, though it could be expanded to read in signals on every Arduino pin, digital and analog, simultaneously.
--this would be lots of work, so for now I'll leave that up to you.
-this code should be able to read in any pulse between approximately 10~20us and 35.79 minutes; I'll let you experiment
 to find the actual shortest pulse you can measure with it

By Gabriel Staples
http://www.ElectricRCAircraftGuy.com/
-My contact info is available by clicking the "Contact Me" tab at the top of my website.
Written: 28 Nov. 2013
Updated: 21 March 2015

Some References:
-to learn how to manipulate some of the low-level AVR code, pin change interrupts, etc, these links will help
--http://www.gammon.com.au/interrupts
--ATmega328 datasheet: http://www.atmel.com/Images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet.pdf
--http://playground.arduino.cc/Main/TimerPWMCheatsheet
--See the *many* very helpful links at bottom of this article: http://www.electricrcaircraftguy.com/2014/01/the-power-of-arduino.html
--reference the Arduino source code, ex:
---C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h
---C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\wiring_digital.c
---C:\Program Files (x86)\Arduino\hardware\arduino\avr\variants\standard\pins_arduino.h
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

/*
Circuits:

Option 1) To measure the pulses from an Arduino PWM pin, ex: pin 5 or 9, connect Pin 5 or 9 (a PWM output) to INPUT_PIN (the pulse reader input)
          -see the setup() function for commanding the PWM output to begin, so you can have something to read in

Option 2) To measure an RC PWM servo-type signal coming from an RC Rx:
-Power the Rx by connecting 5V to + on the Rx, and GND to - on the Rx
-Connect the channel signal you want to measure on the Rx to the INPUT_PIN on the Arduino
*/

#include <eRCaGuy_Timer2_Counter.h>

//macros
#define fastDigitalRead(p_inputRegister, bitMask) ((*p_inputRegister & bitMask) ? HIGH : LOW)

//Global Variables & defines
const byte INPUT_PIN = 12; //you can change this to ANY digital or analog pin, ex: 10, 8, A0, A5, etc, 
                           //EXCEPT A6 and A7 (which exists on the Nano and Pro Mini, for example, and are NOT capable of digital operations)
byte input_pin_bitMask;
volatile byte* p_input_pin_register;
 
//volatile variables for use in the ISR (Interrupt Service Routine)
volatile boolean output_data = false; //the main loop will try to output data each time a new pulse comes in, which is when this gets set true
volatile unsigned long pulseCounts = 0; //units of 0.5us; the input signal high pulse time
volatile unsigned int pd = 0; //units of 0.5us; the pulse period (ie: time from start of high pulse to start of next high pulse)

void setup() 
{
  pinMode(INPUT_PIN,INPUT);

  //configure timer2
  timer2.setup();

  //prepare for FAST digital reads on INPUT_PIN, by mapping to the input register (ex: PINB, PINC, or PIND), and creating a bitMask
  //using this method, I can do digital reads in approx. 0.148us/reading, rather than using digitalRead, which takes 4.623us/reading (31x speed increase)
  input_pin_bitMask = digitalPinToBitMask(INPUT_PIN);
  p_input_pin_register = portInputRegister(digitalPinToPort(INPUT_PIN));
  
  configurePinChangeInterrupts();
  
  //start PWM output (to read in its pulses, for use in Circuit 1 as described above)
  pinMode(5,OUTPUT);
  pinMode(9,OUTPUT);
  analogWrite(5,10); //976.5625Hz, with high pulses of ~1/976.5625 x 10/256 = 40us; Connect pin 5 to INPUT_PIN and open serial monitor & you will see approximately this
  analogWrite(9,128); //490.20Hz, with high pulses of ~1/490.2 x 128/256 = ~1020us; Connect pin 9 to INPUT_PIN and open serial monitor & you will see approximately this
  /*
  PWM Notes:
  -PWM on pins 5 & 6 occurs at 976.5625Hz; see here: http://playground.arduino.cc/Main/TimerPWMCheatsheet
  -PWM on pins 9 & 10 occurs at 490.20Hz  
  */
  
  
  Serial.begin(115200);
  Serial.print(F("Begin waiting for pulses on pin ")); Serial.print(INPUT_PIN);
  Serial.println(F(".\nData will be printed after each pulse is received."));
}

void loop() 
{
  //local variables
  static float pulseTime = 0; //us; the most recent input signal high pulse time
  static float pd_us = 0; //us; the most recent input signal period between pulses
  static float pulseFreq = 0; //Hz, the most recent input signal pulse frequency
  
  if (output_data==true) //if a pulse just came in
  {
    //turn off interrupts, grab copies of volatile data, and re-enable interrupts
    noInterrupts();
    output_data = false; //reset
    unsigned long pulseCountsCopy = pulseCounts; //0.5us units
    unsigned long pdCopy = pd; //0.5us units
    interrupts();
    
    //do calculations
    pulseTime = pulseCountsCopy/2.0; //us
    pd_us = pdCopy/2.0; //us
    pulseFreq = 1000000.0/pd_us; //Hz
    
    //print values 
    //(optionally, add extra code here to not print after EVERY pulse is received, as this can result in serial data coming in excessively fast when pulses come in at a high freq)
    Serial.print(F("pulsetime(us) = ")); Serial.print(pulseTime);
    Serial.print(F(", pd_us(us) = ")); Serial.print(pd_us);
    Serial.print(F(", pulseFreq(Hz) = ")); Serial.println(pulseFreq);
  }
} //end of loop()

////Use macro instead
//boolean fastDigitalRead(volatile byte* p_inputRegister,byte bitMask)
//{
//  return (*p_inputRegister & bitMask) ? HIGH : LOW;
//}

void pinChangeIntISR()
{
  //local variables
  static boolean pin_state_new = LOW; //initialize
  static boolean pin_state_old = LOW; //initialize
  static unsigned long t_start = 0; //units of 0.5us
  static unsigned long t_start_old = 0; //units of 0.5us
  
  pin_state_new = fastDigitalRead(p_input_pin_register,input_pin_bitMask);
  if (pin_state_old != pin_state_new)
  {
    //if the pin state actualy changed, & it was not just noise lasting < ~2~4us
    pin_state_old = pin_state_new; //update the state
    if (pin_state_new == HIGH)
    {
      t_start = timer2.get_count(); //0.5us units
      pd = t_start - t_start_old; //0.5us units, the incoming pulse period
      t_start_old = t_start; //0.5us units; update
    }
    else //pin_state_new == LOW
    {
      unsigned long t_end = timer2.get_count(); //0.5us units
      pulseCounts = t_end - t_start; //0.5us units
      output_data = true;
    }
  }
}

//Interrupt Service Routines (ISRs) for Pin Change Interrupts
//see here: http://www.gammon.com.au/interrupts

//PCINT0_vect is for pins D8 to D13
ISR(PCINT0_vect)
{
  pinChangeIntISR();
}

//PCINT1_vect is for pins A0 to A5
ISR(PCINT1_vect)
{
  pinChangeIntISR();
}

//PCINT2_vect is for pins D0 to D7
ISR(PCINT2_vect)
{
  pinChangeIntISR();
}

void configurePinChangeInterrupts()
{
  //Pin Change Interrupt Configuration
  //see ATmega328 datasheet, ex: pgs. 73-75
  //also see: C:\Program Files (x86)\Arduino\hardware\arduino\avr\variants\standard\pins_arduino.h for the macros used here
  //1st, set flags on the proper Pin Change Mask Register (PCMSK)
  volatile byte* p_PCMSK = (volatile byte*)digitalPinToPCMSK(INPUT_PIN); //pointer to the proper PCMSK register
  *p_PCMSK = _BV(digitalPinToPCMSKbit(INPUT_PIN));
  //2nd, set flags in the Pin Change Interrupt Control Register (PCICR)
  volatile byte* p_PCICR = (volatile byte*)digitalPinToPCICR(INPUT_PIN); //pointer to PCICR
  *p_PCICR |= _BV(digitalPinToPCICRbit(INPUT_PIN));
  
//  //ex: to use digital pin 8 as the INPUT_PIN:
//  //turn on PCINT0_vect Pin Change Interrupts (for pins D8 to D13); see datasheet pg. 73-75.
//  //1st, set flags on the proper Pin Change Mask Register
//  PCMSK0 = 0b00000001; //here I am setting Bit0 to a 1, to mark pin D8's pin change register as on; for pin mapping see here: http://arduino.cc/en/Hacking/PinMapping168
//  //2nd, set flags in the Pin Change Interrupt Control Register
//  PCICR |= 0b00000001; //here I am turning on the pin change vector 0 interrupt, for PCINT0_vect, by setting the right-most bit to a 1
}

