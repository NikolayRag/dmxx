/*
Ledlut is 8-bit to higher resolution gamma-corrected remap function.
It is tuned originally for 11bit 7.8kHz output, considered ok for flickerless led,
 tested with real camera response.
The main idea is to offset [1, 255] input range (DMX resolution)
 enough to make output values consecutive at start, when input is gamma-corrected
 and mapped back to full output range [1,2047], so input virtually expands at start
 and acts as if values lower than virtual offset are under PWM resolution.

That needs v(N+1)-v(N)>step for equations: 
v1 = ((1+n)/(max+n))^2 (/step)
v2 = ((2+n)/(max+n))^2 (/step)

This approach effectively utilizes lowest output PWM values, which are visually
 the most different.


Proper LUTOFFSET for INMAX/OUTBITS resolution with LUTWARMUP<=1:

    OUT    9 10 11 12 13 14 15 16
IN      -------------------------
127     | 12  4  1  0  0  0  0  0
254/255 | 95 30 11  4  1  0  0  0
*/


#ifndef INMAX
#define INMAX 254 //suitable for doubled 127-based midi events
#endif

#ifndef OUTBITS
#define OUTBITS 11
#endif

#ifndef LUTOFFSET
#define LUTOFFSET 11 //pick for particular INMAX and OUTBITS change
#endif

#ifndef LUTWARMUP
#define LUTWARMUP 0 //output offset for 0
#endif

#ifndef LUT1TO
#define LUT1TO 1.49 //Map input 1 to. Take in account value is up to be rounded.
#endif


//return adjusted value with 0-offset
float lutUp(float v, int offset=LUTOFFSET) {
  float remap01Offset = float(v+offset)/float(INMAX+offset);
  return remap01Offset*remap01Offset; //actual lut
}


//map fn:  low2 + (value - low1) * (high2 - low2) / (high1 - low1)
float REFMAX = pow(2,OUTBITS)-1;
float REFMIN = LUTWARMUP+LUT1TO;
float REF1 = lutUp(1);
float REFMAP = (REFMAX-REFMIN)/(1.-REF1);

//map input 0-1 to output range, respecting 1-value remap to LUT1TO
#define LUT_(v) REFMIN+(lutUp(v)-REF1)*REFMAP 
#define LUT(v) v? v<INMAX? LUT_(v) :REFMAX :LUTWARMUP //guaranteed [+0,+1,.., max]
#define LUTI(v) round(LUT(v))
