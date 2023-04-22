#include "ledlut/ledlut.h"

#include <DMXSerial.h>
#include "GyverPWM.h"


#define CH1 9
#define CH2 10

#define DMXBASE 4
#define _BOARDOFFS 0
//#define _BOARDOFFS 2 //for 2nd board, 3rd channel



//ledlut override
//#define LUTBITS 11



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  DMXSerial.init(DMXReceiver);

  
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  PWM_resolution(CH1, LUTBITS, FAST_PWM);
  PWM_resolution(CH2, LUTBITS, FAST_PWM);
}





//#define BLINKAT 10000
long nProgress= 0;

void loop() {
  byte inC1 = DMXSerial.read(DMXBASE+1+_BOARDOFFS);
  PWM_set(CH1, LUTI(inC1));
  byte inC2 = DMXSerial.read(DMXBASE+2+_BOARDOFFS);
  PWM_set(CH2, LUTI(inC2)); //NC for 2nd DMX



//progress blink
#ifdef BLINKAT
  if (nProgress==10){
    digitalWrite(LED_BUILTIN,0);
  }
  if (nProgress==BLINKAT){
    digitalWrite(LED_BUILTIN,1);
    nProgress= 0;
  }
  nProgress++;
#endif
}
