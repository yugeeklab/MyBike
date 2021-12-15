/*
  Keyboard test

  For the Arduino Leonardo, Micro or Due

  Reads a byte from the serial port, sends a keystroke back.
  The sent keystroke is one higher than what's received, e.g. if you send a,
  you get b, send A you get B, and so forth.

  The circuit:
  - none

  created 21 Oct 2011
  modified 27 Mar 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/KeyboardSerial
*/

#include "Keyboard.h"
#include <Wire.h>
#include <MechaQMC5883.h>

MechaQMC5883 qmc;

float heading, headingDegrees, headingFiltered, calHeadingFiltered;

void setup() {
  pinMode(4, INPUT);
  // initialize control over the keyboard:
  Wire.begin();
  Serial.begin(9600);
  Keyboard.begin();
  qmc.init();
  initDirection(&calHeadingFiltered);
}

void loop() {
  int x, y, z;
  qmc.read(&x, &y, &z);

  heading = atan2(z, x);
  if(heading <0) heading += 2*PI;
  if(heading > 2*PI)heading -= 2*PI;

  headingDegrees = heading * 180/PI; // The heading in Degrees unit
  
  // Smoothing the output angle / Low pass filter 
  headingFiltered = headingFiltered*0.85 + headingDegrees*0.15;
  Serial.println(headingFiltered - calHeadingFiltered);
  if (digitalRead(4) == LOW ) {
    // do nothing until pin 2 goes low
    Keyboard.press(32); 
  }else {
    Keyboard.release(32);
  }

  if(headingFiltered - calHeadingFiltered < 0) {
    Keyboard.press(KEY_LEFT_ARROW);
    delay(50);
    Keyboard.release(KEY_LEFT_ARROW);
  }else {
    Keyboard.press(KEY_RIGHT_ARROW); 
    delay(50);
    Keyboard.release(KEY_RIGHT_ARROW);
  }
}

void initDirection(float* calHeadingFiltered) {
  int x, y, z;
  float sumX = 0, sumY = 0, sumZ = 0;
  for(int i = 0; i < 5; i++) {
     qmc.read(&x, &y, &z);
     sumX += x;
     sumY += y;
     sumZ += z;
  }

  float heading = atan2(sumZ/5, sumX/5);
  if(heading <0) heading += 2*PI;
  if(heading > 2*PI)heading -= 2*PI;

  float headingDegrees = heading * 180/PI; // The heading in Degrees unit
  
  *calHeadingFiltered = headingDegrees;
}
