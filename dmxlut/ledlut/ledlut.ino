/*
Demo case.
Given LUT settings, produces 

#include "ledlut.h"

//#define LUTMAX 127
//#define LUTBITS 16
//#define LUTDEGREE 2
//#define LUTOFFSET 0
//#define LUTWARMUP 1



void setup() {
  Serial.begin(57600);

  Serial.println(REFMAX,10);
  Serial.println(REFMIN,10);
  Serial.println(REF1,10);
  Serial.println(REFMAP,10);
  Serial.println(millis());

  float sum= 0;
  for (float i=0; i<=LUTMAX+1; i+=.5){
    sum+= LUTI(i);

    Serial.print(i);
    Serial.print("\t > ");
    Serial.print(LUTI(i));
    Serial.print("\t (");
    Serial.print(LUT(i), 8);
    Serial.print(")\n");
  }

  Serial.println(sum);
  Serial.println(float(millis()));
}



void loop() {
}
