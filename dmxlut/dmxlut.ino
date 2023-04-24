/*
dmxx setup pins:

2-8,11,12
  DMX base address LSb. Block of 4(7 hires) used.

14,15
  Output resolution LSb, stands for (11,12,13,16)

16
  Warmup (always minimally lit at 0)

17,18
  Filter LSb, stands for [minimal, severe, decay, none]

19
  Board primary(base+1,base+2)/secondary(base+3,base+4).


Setup pins affects state at reset.
*/

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
// +++ setup

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  pinMode(19, INPUT_PULLUP);

  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);

  pinMode(16, INPUT_PULLUP);

  pinMode(17, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);



  DMXBASE =
    !digitalRead(2) +
    !digitalRead(3) *2 +
    !digitalRead(4) *4 +
    !digitalRead(5) *8 +
    !digitalRead(6) *16 +
    !digitalRead(7) *32 +
    !digitalRead(8) *64 +
    !digitalRead(11) *128 +
    !digitalRead(12) *256;


  DMX1 = DMXBASE +1;
  DMX2 = DMXBASE +2;

  if (!digitalRead(19)){
    DMX1 = DMXBASE +3;
    DMX2 = DMXBASE +4;
  }


  #define LUTTABLE (int[]){11, 12, 13, 16}
  LUTBITS = LUTTABLE[!digitalRead(14) +!digitalRead(15)*2];
  LUTOFFSET = LUTOFFSET254(LUTBITS);

  LUTWARMUP = !digitalRead(16);

  #define FILTERTABLE (float[]){.5, .1, .01, 1.}
  FILTER = FILTERTABLE[!digitalRead(17) +!digitalRead(16)*2];

// --- setup
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
