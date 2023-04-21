/*
Ledlut is 8-bit to higher resolution gamma-corrected remap function.
It is tuned originally for 11bit 7.8kHz output, considered ok for flickerless led,
 tested with real camera response.
The main idea is to offset [1, 255] input range (DMX resolution), which then is gamma-corrected
 and mapped back to full output range [1,2047], so the output PWM starts with consecutive values [1,2,...]
In other words, input virtually expands at start and acts as if values lower than virtual offset
 are under PWM resolution.

This approach effectively utilizes lowest output PWM values, which are visually the most different.

That needs v(N+1)-v(N)>1 for begin values, respect to integer round up:
v(N) = ((N+n)/(max+n))^2 /step


Proper LUTOFFSET for LUTMAX/OUTBITS resolution with LUTWARMUP<=1:

LUTDEGREE = 2
    OUT    9 10 11 12 13 14 15 16
IN      -------------------------
127     | 12  4  1  0  0  0  0  0
254/255 | 95 30 11  4  1  0  0  0


LUTDEGREE = 2.2
    OUT    9 10 11 12 13 14 15 16
IN      -------------------------
127     | 19  8  3  0  0  0  0  0
254/255 |127 45 19  8  3  1  0  0
*/


#ifndef LUTMAX
#define LUTMAX 254 //suitable for doubled 127-based midi events
#endif

#ifndef LUTBITS
#define LUTBITS 11 //PWM resolution
#endif

#ifndef LUTDEGREE
#define LUTDEGREE 2 //degree of LUT curve
#endif

#ifndef LUTOFFSET
#define LUTOFFSET 11 //pick for particular LUTMAX and LUTBITS change
#endif

#ifndef LUTWARMUP
#define LUTWARMUP 0 //output offset for 0
#endif

#ifndef LUT1TO
#define LUT1TO 1.49 //Map input 1 to. Take in account value is up to be rounded.
#endif


//actual lut function
#define lutFn(v) pow(v, LUTDEGREE)
#define lutUp(v) lutFn(float(v+LUTOFFSET)/float(LUTMAX+LUTOFFSET)) //offset value with applied lut


//map fn:  low2 + (value - low1) * (high2 - low2) / (high1 - low1)
#define REFMAX pow(2,LUTBITS)-1
#define REFMIN LUTWARMUP+LUT1TO
#define REF1 lutUp(1)
#define REFMAP (float(REFMAX)-float(REFMIN))/(1.-float(REF1))

//map input 0-1 to output range, respecting 1-value remap to LUT1TO
#define LUT_(v) max(REFMIN+(lutUp(v)-REF1)*REFMAP,LUTWARMUP) //remap and 0-clamp
#define LUT(v) v? v<LUTMAX? LUT_(v) :REFMAX :LUTWARMUP //guaranteed [+0,+1,.., max]
#define LUTI(v) round(LUT(v))
