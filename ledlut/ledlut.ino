//#define INMAX 127
//#define LUTOFFSET 1
//#define LUTWARMUP 1

#include "ledlut.h"

void setup() {
  Serial.begin(57600);

  Serial.print("lut(1) = ");
  Serial.println(lutUp(1), 8);
  for (float i=0; i<=INMAX+1; i+=.5){
    Serial.print(i);
    Serial.print("\t > ");
    Serial.print(LUTI(i));
    Serial.print("\t (");
    Serial.print(LUT(i), 8);
    Serial.print(")\n");
  }
}



void loop() {
}
