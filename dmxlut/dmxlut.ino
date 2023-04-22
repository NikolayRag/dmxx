#include "ledlut/ledlut.h"

#include <DMXSerial.h>
#include "GyverPWM.h"


#define CH1 9
#define CH2 10

#define DMXBASE 4
#define _BOARDOFFS 0
//#define _BOARDOFFS 2 //for 2nd board, 3rd channel


#define FILTER .5


//ledlut override
#define LUTWARMUP 1


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  DMXSerial.init(DMXReceiver);

  
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  PWM_resolution(CH1, LUTBITS, FAST_PWM);
  PWM_resolution(CH2, LUTBITS, FAST_PWM);
}



float outC1 = 0;
float outC2 = 0;


//#define BLINKAT 10000
long nProgress= 0;

void loop() {
  byte inC1 = DMXSerial.read(DMXBASE+1+_BOARDOFFS);
  byte inC2 = DMXSerial.read(DMXBASE+2+_BOARDOFFS);

  outC1 += (inC1-outC1) *FILTER;
  outC2 += (inC2-outC2) *FILTER;

  PWM_set(CH1, LUTI(outC1));
  PWM_set(CH2, LUTI(outC2)); //NC for 2nd DMX



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
