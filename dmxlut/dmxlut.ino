/*
Setup pins affects state at reset.

issues:
  Bit 1 is always being 0 as pin 2 interfere when linked to other board.

*/

/*
  DMX base.
  Block of 4(7 hires) used.
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
  Output resolution LSb, stands for (11,12,13,16)
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


#include "ledlut/ledlut.h"

#include <DMXSerial.h>

#include "GyverPWM.h"



#define CH1 9
#define CH2 10

int DMXBASE = 0;
int DMX1 = 1;
int DMX2 = 2;


float FILTERTABLE[8]; //pow(0.667, n*4) sequence
float FILTER = 1.;

void setFilter(byte filterIdx){
  FILTER = FILTERTABLE[filterIdx];
}


void setResolution(byte res){
  LUTBITS = res;
  LUTOFFSET = LUTOFFSET254(LUTBITS);

  //need to reset pwm range
  PWM_resolution(CH1, LUTBITS, FAST_PWM);
  PWM_resolution(CH2, LUTBITS, FAST_PWM);
}


void setup() {
  for (byte i=0; i<8; i++)
    FILTERTABLE[i] = pow(.667, i*4);


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


//notice DMX address range is [1,511]
  DMXBASE = 1 +
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


  setResolution(
    (byte[]){11, 12, 13, 16}[
      !digitalRead(SETUP_RESOLUTION_BIT1) +
      !digitalRead(SETUP_RESOLUTION_BIT2)*2
    ]
  );


  LUTWARMUP = !digitalRead(SETUP_WARMUP);


  setFilter(
    (byte[]){2, 5, 7, 0}[ //standing to [some, noticable, sloooow, none]
      !digitalRead(SETUP_FILTER_BIT1) +
      !digitalRead(SETUP_FILTER_BIT2)*2
    ]
  );

// --- setup



  DMXSerial.init(DMXReceiver);
}



float outC1 = 0;
float outC2 = 0;

byte baseHold = 0;
byte baseCurrent = 0;

byte inC1 = 0;
byte inC2 = 0;


// #define BLINKAT 10000
long nProgress= 0;



void applyCfg(byte _baseCmd, byte _baseArg) {
    switch (_baseCmd){
      case 0: //mode
//  todo 3 (feature) +0: modes
//  * 0 (default): Lowres [0-254] with LUT, 1 DMX channel per output
//  * 1  Lowres [0-255] with LUT, 1 DMX channel per output
//  * 2: Lowres without LUT, most like general 8bit but with filtering
//  * 4: Hires without LUT, 2 DMX channels MSB per output; In-to-out is mapped with respect to PWM resolution
        break;

      case 2: //resolution
//  * [bbb]: PWM resolution, 0-7 stand for [11,12,13,14,15,16,16,16] bits
//  * [w]: LUT warmup switch, making device being lit minimally all the time
        byte resI = (_baseArg & 0b00011100) >> 2;
        setResolution(
          (byte[]){11, 12, 13, 14, 15, 16,16,16}[resI]
        );

        LUTWARMUP = (_baseArg & 0b00000010) >> 1;

        break;

      case 4: //filter
//  * [fff] is a filter values table index, from 0 for instant switch to 7 for visually unnoticable gradient
//  todo 4 (feature) +0: filter types/curves
//  * [cc] *reserved for curve type*
        setFilter( (_baseArg & 0b00011100) >> 2 );
        break;
    }
}



void loop() {
  baseCurrent = DMXSerial.read(DMXBASE);
  inC1 = DMXSerial.read(DMX1);
  inC2 = DMXSerial.read(DMX2);

//  todo 1 (feature) +0: review config protocol
  if (baseCurrent != baseHold){ //config change
    applyCfg(
      baseCurrent & 0b00000111,
      (baseCurrent & 0b11111000) >> 3
    );
    
    baseHold = baseCurrent;
  }


//  todo 2 (feature) +0: definable transition curves
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
