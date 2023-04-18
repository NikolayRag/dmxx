/*
Ledlut is 8-bit to higher resolution gamma-corrected remap function.
It is tuned originally for 11bit 7.8kHz output, considered ok for flickerless led,
 tested with real camera response.
The main idea is to offset [1, 255] input range (DMX resolution)
 enough to make its values sequental at start, when gamma-corrected and mapped back
 to full output range [1,2047].
 That acts as if values lower than virtual offset are under PWM resolution.
*/

/*
That needs v2>v1+step for equations: 
v1 = ((1+n)/(255+n))^2 (/step)
v2 = ((2+n)/(255+n))^2 (/step)
which requires n>=11 for 1/2047 step ([4.17, 4.89, 5.67, 6.51, 7.41, 8.36, 9.38,..]
 [0,2047]-based start values), then mapped back from [v1=4.17, 2047]
 to [1.5-, 2047] for normal or [2.5-, 2047] for always warmup mode.

That is: 2047-(1-vLut)*c
; c=  2047/(1-(v1-1.495/2047)) =2049.67446857
; cW= 2047/(1-(v1-2.495/2047)) 
; v1(n11b255) =(12/266)^2 =0.002035163

For 128-based input, suitable n>=1
; v1(n1b127)= 0.000244140625
*/


#define BMAX 254 //suitable for doubled 127-based midi events
#define BIT 11
#define LMAX pow(2,BIT)-1
#define LMAXF float(LMAX)

#define LUTOFFSET 11 //pick for particular BMAX and BIT change
#define LUTWARMUP 0 //output offset for 0
#define LUT1TO (float(LUTWARMUP)+1.495) //Map input 1 to. Take in account value is up to be rounded.


//return adjusted value with 0-offset
float lutUp(float v, int offset=LUTOFFSET) {
  float remap01Offset = float(v+offset)/float(BMAX+offset);
  return remap01Offset*remap01Offset; //actual lut
}


float REFMAP = LMAXF/(1.-(lutUp(1)-LUT1TO/LMAXF));

//map input 0-1 to output range, respecting 1-value remap to LUT1TO
#define LUT_(v) LMAXF-(1.-lutUp(v))*REFMAP 
#define LUT(v) v? v<BMAX? LUT_(v) :LMAX :LUTWARMUP
#define LUTI(v) v? v<BMAX? round(LUT_(v)) :LMAX :LUTWARMUP

