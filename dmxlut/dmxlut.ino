#include "ledlut/ledlut.h"

#include <DMXSerial.h>
#include "GyverPWM.h"


#define CH1 9
#define CH2 10

int DMXBASE = 0;
int DMX1 = 1;
int DMX2 = 2;


float FILTER = .5;



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

byte inC1 = 0;
byte inC2 = 0;


//#define BLINKAT 10000
long nProgress= 0;

void loop() {
  inC1 = DMXSerial.read(DMX1);
  inC2 = DMXSerial.read(DMX2);

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
