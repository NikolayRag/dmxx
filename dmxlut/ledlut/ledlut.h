/*
Ledlut is 8-bit to higher resolution gamma-corrected remap function.
It is tuned originally for 11bit 7.8kHz Arduino output, considered ok for flickerless led,
 tested with real camera response.
The main idea is to offset [1, 255] input range (DMX resolution), which then is gamma-corrected
 and mapped back to full output range [1,2047], so the output PWM starts with consecutive values [1,2,...]
In other words, input virtually expands at start and acts as if values lower than virtual offset
 are under PWM resolution.

This approach effectively utilizes lowest output PWM values, which are visually the most different.

That needs v(N+1)-v(N)>1 for lowest values, with respect of integer round up.
:
v(N) = ((N+n)/(max+n))^2 /step


Proper handpicked LUTOFFSET for LUTMAX/OUTBITS resolution with LUTWARMUP<=1:

LUTDEGREE = 2
    OUT    9 10 11 12 13 14 15 16
IN      -------------------------
127     | 12  4  1  0  0  0  0  0
254/255 | 95 30 11  4  1  0  0  0


LUTDEGREE = 2.2, for comparison
    OUT    9 10 11 12 13 14 15 16
IN      -------------------------
127     | 19  8  3  1  0  0  0  0
254/255 |127 45 19  8  3  1  0  0
*/


/*
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
*/
int LUTMAX = 254; //suitable for doubled 127-based midi events
int LUTBITS = 11; //PWM resolution
int LUTDEGREE = 2; //degree of LUT curve
int LUTOFFSET = 11; //pick for particular LUTMAX and LUTBITS change
int LUTWARMUP = 0; //output offset for 0

#ifndef LUT0TOL
#define LUT0TOL 0.1 //Warmup beyond this point, meaningful at filtering
#endif

#ifndef LUT1TO
#define LUT1TO 1.49 //Map input 1 to. Take in account value is up to be rounded.
#endif


//preset helper used with LUTMAX=254
#define LUTOFFSET254(b) (int[]){0,0,0,0,0,0,0,0,0,95,30,11,4,1,0,0,0}[b]

//actual lut function
//  todo 1 (general, lut, feature) +0: Triange numbers lut
#define lutTri(v) .5*(v*(v+1)) //triangle numbers lut: y=(x(x+1))/2
#define lutFn(v) pow(v, LUTDEGREE)
#define lutUp(v) lutFn(float(v+LUTOFFSET)/float(LUTMAX+LUTOFFSET)) //offset value with applied lut


//map fn:  low2 + (value - low1) * (high2 - low2) / (high1 - low1)
#define REFMAX pow(2,LUTBITS)-1
#define REFMIN LUTWARMUP+LUT1TO
#define REF1 lutUp(1)
#define REFMAP (float(REFMAX)-float(REFMIN))/(1.-float(REF1))

//map input 0-1 to output range, respecting 1-value remap to LUT1TO
// -todo 11 (check) +0: LUT_() redundantly clamps to warmup for some reason, need check
#define LUT_(v) max(REFMIN+(lutUp(v)-REF1)*REFMAP,LUTWARMUP) //float raw remap
#define LUT(v) v>LUT0TOL? (v<LUTMAX? LUT_(v) :REFMAX) :LUTWARMUP //float [warmup,+1,.., max] clamped
#define LUTI(v) round(LUT(v)) //int clamped
