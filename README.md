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
Default input is clamped at 254 - that makes it easy the remap of 7bit resolution input (0-127)
with cost of topmost 1/256 step.

Default output resolution is 11bit depth, at maximum of 16.  
PWM frequence is in inverse ratio with resolution, being 7.8Khz at 11bit,
which is ballanced solution for smooth values switching and reduced flicker,
even when captured back on camera at relatively short shutter (1/1000).

Issue: Noticable steps still will be visible when brightness is changed at low values with very smooth filtering
 on low resolution. Use up to 16bit resolution for that cases.
 


Overall LUT function is designed to maximize utilization of lowest consecutive output values: [0,1,2,..]


todo: *`dmxx` uses 4 (default) or 7 channels per device, which are [Control, R, G, B] or [Control, R, r, G, g, B, b]*



#### Filtered transitions

Filtering is applied to make smooth output transition, mostly visible when switching
 between adjacent low values on bright emitters.

Default transition time is taken so, that switching is smooth visually but no more - 
to leave fast switching of significant ranges available at the same time.

todo: *Subject to change to controllable curve*



#### todo: *High-resolution input support*

Mode to allow more than 8bit input per channel.  
This gives ability to switch filtering and LUT off, to control output directly.



#### Device setup over root DMX channel

`dmxx` is accepting configuration values on root (+0) DMX channel.

V1 protocol:  
Byte recieved is set of [vvvvvccc] bits, where ccc is MSB command and vvvvv is MSB argument.


Controllable settings and their defaults are, for [ccc]:

000: PWM resolution (11 bits). Argument stands for bits value, clamped by [11,16]
-* LUT: maximum input value (254)
-* LUT: power function (2), output gamma
010: LUT warmup value (0), the constant added to output,
making device being lit minimally all the time. Argument is offset, most usable for 0 and 1
-* LUT: virtual offset, used to tune mapping of lowest values, see table
-* LUT: button (1), effectively switch LUT on or off

100: Filter index from table of transition factors. Argument is from 0 for instant switch to 31 for visually unnoticable gradient

-* Hires: mode (0), for 1/2 DMX channels (input bytes) per output

