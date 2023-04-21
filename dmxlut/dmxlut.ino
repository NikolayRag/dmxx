#include "ledlut/ledlut.h"

#include <DMXSerial.h>
#include "GyverPWM.h"

#define OUT1 9
#define OUT2 10

#define DMXBASE 4
//#define DMX2 //for 2nd board, 3rd channel


//ledlut override
//#define LUTBITS 11



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  DMXSerial.init(DMXReceiver);

  
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  PWM_resolution(OUT1, LUTBITS, FAST_PWM);
  PWM_resolution(OUT2, LUTBITS, FAST_PWM);
}



#ifdef DMX2
byte _boardOffs= 2;
#else
byte _boardOffs= 0;
#endif


long nProgress= 0;
#define BLINKAT 100000

void loop() {
  byte inC1 = DMXSerial.read(DMXBASE+1+_boardOffs);
  PWM_set(OUT1, LUTI(inC1));
  byte inC2 = DMXSerial.read(DMXBASE+2+_boardOffs);
  PWM_set(OUT2, LUTI(inC2)); //NC for 2nd DMX



//progress blink
  if (nProgress==BLINKAT){
    digitalWrite(LED_BUILTIN,1);
  }
  if (nProgress==BLINKAT*2){
    digitalWrite(LED_BUILTIN,0);
    nProgress= 0;
  }
  nProgress++;
}
