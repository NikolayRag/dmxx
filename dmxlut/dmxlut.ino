/*
Setup pins affects state at reset.
*/

/*
  DMX base.
  Block of 4(7 hires) used.
  issue: Bit 1 is always being 0 as pin 2 possibly interfere with int0
*/
#define SETUP_BASE_BIT1 2
#define SETUP_BASE_BIT2 3
#define SETUP_BASE_BIT3 4
#define SETUP_BASE_BIT4 5
#define SETUP_BASE_BIT5 6
#define SETUP_BASE_BIT6 7
#define SETUP_BASE_BIT7 8
#define SETUP_BASE_BIT8 11
#define SETUP_BASE_BIT9 12


/*
  Output resolution LSb, stands for (11,12,13,14,15,16)
*/
#define SETUP_RESOLUTION_BIT1 14
#define SETUP_RESOLUTION_BIT2 15
#define SETUP_RESOLUTION_BIT3 16

/*
  Filter LSb, stands for [minimal, severe, decay, none]
*/
#define SETUP_FILTER_BIT1 17
#define SETUP_FILTER_BIT2 18

/*
  Warmup (always minimally lit at 0)
*/
#define SETUP_WARMUP 19

/*
  Board primary(base+1,base+2)/secondary(base+3,base+4).
*/
#define SETUP_SECONDARY 21


//#define TEST


#include "ledlut/ledlut.h"


#ifndef TEST
#include <DMXSerial.h>
#endif

#include "GyverPWM.h"



#define CH1 9
#define CH2 10

int DMXBASE = 0;
int DMX1 = 1;
int DMX2 = 2;


float FILTER = .5;



void setup() {
// +++ setup
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);


//  pinMode(SETUP_BASE_BIT1, INPUT_PULLUP); //int0 possible interference
  pinMode(SETUP_BASE_BIT2, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT3, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT4, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT5, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT6, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT7, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT8, INPUT_PULLUP);
  pinMode(SETUP_BASE_BIT9, INPUT_PULLUP);


  pinMode(SETUP_RESOLUTION_BIT1, INPUT_PULLUP);
  pinMode(SETUP_RESOLUTION_BIT2, INPUT_PULLUP);
  pinMode(SETUP_RESOLUTION_BIT3, INPUT_PULLUP);

  pinMode(SETUP_WARMUP, INPUT_PULLUP);

  pinMode(SETUP_FILTER_BIT1, INPUT_PULLUP);
  pinMode(SETUP_FILTER_BIT2, INPUT_PULLUP);


  pinMode(SETUP_SECONDARY, INPUT_PULLUP);


  delay(100); //remove possible interference from state switing


  DMXBASE =
//    !digitalRead(SETUP_BASE_BIT1) *1 +
    !digitalRead(SETUP_BASE_BIT2) *2 +
    !digitalRead(SETUP_BASE_BIT3) *4 +
    !digitalRead(SETUP_BASE_BIT4) *8 +
    !digitalRead(SETUP_BASE_BIT5) *16 +
    !digitalRead(SETUP_BASE_BIT6) *32 +
    !digitalRead(SETUP_BASE_BIT7) *64 +
    !digitalRead(SETUP_BASE_BIT8) *128 +
    !digitalRead(SETUP_BASE_BIT9) *256;


  DMX1 = DMXBASE +1;
  DMX2 = DMXBASE +2;

  if (!digitalRead(SETUP_SECONDARY)){
    DMX1 = DMXBASE +3;
    DMX2 = DMXBASE +4;
  }


  #define LUTTABLE (int[]){11, 12, 13, 14, 15, 16, 16, 16}
  LUTBITS = LUTTABLE[
    !digitalRead(SETUP_RESOLUTION_BIT1) +
    !digitalRead(SETUP_RESOLUTION_BIT2)*2 +
    !digitalRead(SETUP_RESOLUTION_BIT3)*4
  ];
  LUTOFFSET = LUTOFFSET254(LUTBITS);


  LUTWARMUP = !digitalRead(SETUP_WARMUP);


  #define FILTERTABLE (float[]){.5, .1, .01, 1.}
  FILTER = FILTERTABLE[
    !digitalRead(SETUP_FILTER_BIT1) +
    !digitalRead(SETUP_FILTER_BIT2)*2
  ];

// --- setup



#ifdef TEST
  Serial.begin(57600);

  Serial.println(DMXBASE,10);
  Serial.println(DMX1,10);
  Serial.println(DMX2,10);
  Serial.println(LUTBITS,10);
  Serial.println(LUTOFFSET,10);
  Serial.println(LUTWARMUP,10);
  Serial.println(FILTER,10);
#endif



#ifndef TEST
  DMXSerial.init(DMXReceiver);
#endif

  
  PWM_resolution(CH1, LUTBITS, FAST_PWM);
  PWM_resolution(CH2, LUTBITS, FAST_PWM);
}



float outC1 = 0;
float outC2 = 0;

byte inC1 = 0;
byte inC2 = 0;


// #define BLINKAT 10000
long nProgress= 0;

void loop() {

#ifndef TEST
  inC1 = DMXSerial.read(DMX1);
  inC2 = DMXSerial.read(DMX2);
#endif

  outC1 += (inC1-outC1) *FILTER;
  outC2 += (inC2-outC2) *FILTER;

  PWM_set(CH1, LUTI(outC1));
  PWM_set(CH2, LUTI(outC2));



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
