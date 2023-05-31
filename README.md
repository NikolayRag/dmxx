# dmxx

#### High-frequency and high-resolution DMX RGB driver based on Arduino.

`dmxx` utilizes higher than 8bit (DMX) PWM resolution to distribute light energy with LUT mapping.

That makes some visual and management improvements compared to almost any regular 8bit driver:

* Natural light sensing, instead of decaying illumination growth as input increases
* Virtually eliminated perceptible warmup (0 mapped to 1)
* Removed noticable brightness step while switching between almost every adjacent values, with filtered transition
* todo: *High-resolution input support*
* Device setup over DMX itself for variety of parameters


### Brief definitions:

8bit input values are mapped to output resolution using gamma mapping, 2 by default.  
By default input is clamped at 254 - that makes it easy the remap of 7bit resolution input (0-127)
with cost of topmost 1/256 step.

Default output resolution is 11bit depth, at maximum of 16.  
PWM frequence is in inverse ratio with resolution, being 7.8Khz at 11bit,
which is ballanced solution for smooth values switching and reduced flicker,
even when captured back on camera at relatively short shutter (1/1000).



Overall LUT function is designed to maximize utilization of lowest consecutive output values: [0,1,2,..]


todo: *`dmxx` uses 4 (default) or 7 channels per device, which are [Control, R, G, B] or [Control, R, r, G, g, B, b]*



#### Filtered transitions

Filtering is applied to make smooth output transition.

Default transition time is taken so, that switching is smooth visually but no more - 
to leave fast switching of significant ranges available at the same time.

Issue: Noticable steps still will be visible when brightness is changed at low values with very smooth filtering
 on low resolution. Use up to 16bit resolution for that cases.



todo: *Subject to change to controllable curve*



#### todo: *High-resolution input support*

Mode to allow more than 8bit input per channel.  
This gives ability to switch filtering and LUT off, to control output directly.



#### Device setup over root DMX channel

`dmxx` is accepting configuration values on root (+0) DMX channel.

Protocol (v1):  
Byte recieved is set of [vvvvvccc] MSB bits, with [ccc] Command and [vvvvv] Argument.


Controllable settings and their defaults are, by Command:

*2: LUT resolution, warmup and clip. Argument is [bbbwc] where
** [bbb]: Bits, 0-7 stand for [11,12,13,14,15,16,16,16]
** [w]: LUT warmup switch, making device being lit minimally all the time
** [c]: LUT clamp, maximum input value is set to 254 instead

*2: Mode. Argument is one of: 
** 0 (default): Lowres with LUT, 1 DMX channel per output
** 1: Lowres without LUT, 1 DMX channel per output; 
** 2: Hires without LUT, 2 DMX channels MSB per output; direct in-to-out mapping

*4: Filter index from table of transition factors. Argument is from 0 for instant switch to 31 for visually unnoticable gradient
