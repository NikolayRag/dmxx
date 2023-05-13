/*
Setup pins affects state at reset.
*/

/*
  DMX base.
  Block of 4(7 hires) used.
  issue: Bit 1 is always being 0 as pin 2 interfere when linked to other board
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

/*
  Filter LSb, stands for [minimal, severe, decay, none]
*/
#define SETUP_FILTER_BIT1 16
#define SETUP_FILTER_BIT2 17

/*
  Warmup (always minimally lit at 0)
*/
#define SETUP_WARMUP 18

/*
  Board primary(base+1,base+2)/secondary(base+3,base+4).
*/
#define SETUP_SECONDARY 19


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


float FILTERTABLE[32]; //pow(0.8, n) sequence
float FILTER = .5;

void setFilter(byte filterIdx){
  FILTER = FILTERTABLE[filterIdx];
}


void setResolution(byte res){
  LUTBITS = res;
  LUTOFFSET = LUTOFFSET254(LUTBITS);
}


void setup() {
  for (byte i=0; i<32; i++)
    FILTERTABLE[i] = pow(.8, i);


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

  pinMode(SETUP_WARMUP, INPUT_PULLUP);

  pinMode(SETUP_FILTER_BIT1, INPUT_PULLUP);
  pinMode(SETUP_FILTER_BIT2, INPUT_PULLUP);


  pinMode(SETUP_SECONDARY, INPUT_PULLUP);


  delay(100); //remove possible interference from linked pins (pin2 not fixed)


  DMXBASE =
//    !digitalRead(SETUP_BASE_BIT1) *1 +  //int0 possible interference
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


  #define LUTTABLE (int[]){11, 12, 13, 16}
  setResolution(LUTTABLE[
    !digitalRead(SETUP_RESOLUTION_BIT1) +
    !digitalRead(SETUP_RESOLUTION_BIT2)*2
    ]
  );


  LUTWARMUP = !digitalRead(SETUP_WARMUP);


  setFilter(
    (byte[]){7, 17, 31, 0}[ //values closest to [.2, .02, .001, 1], standing to [some, noticable, sloooow, none]
      !digitalRead(SETUP_FILTER_BIT1) +
      !digitalRead(SETUP_FILTER_BIT2)*2
    ]
  );

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

byte baseHold = 0;
byte inBase = 0;
byte baseArg = 0;

byte inC1 = 0;
byte inC2 = 0;


// #define BLINKAT 10000
long nProgress= 0;

void loop() {

#ifndef TEST
  inBase = DMXSerial.read(DMXBASE);

  inC1 = DMXSerial.read(DMX1);
  inC2 = DMXSerial.read(DMX2);
#endif

  if (inBase != baseHold){ //setup change
    baseHold = inBase;

    baseArg = (inBase & 0b11111000) >> 3;

    switch ((inBase & 0b00000111) >> 5){
      case 0: //resolution
        setResolution(
          max( min(baseArg,16), 11)
        );
        break;

      case 2: //warmup
        LUTWARMUP = baseArg;
        break;

      case 4: //filter
        setFilter( baseArg );
        break;
    }
  }


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
