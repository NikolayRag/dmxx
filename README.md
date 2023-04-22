# dmxx

#### High-frequency and high-resolution DMX RGB driver based on Arduino.

`dmxx` utilizes higher than 8bit (DMX) PWM resolution to distribute light energy with LUT mapping.

That makes some visual and management improvements compared to almost any regular 8bit driver:

* Natural light sensing, instead of decaying illumination growth as input increases
* Virtually eliminated perceptible warmup (0 mapped to 1)
* Removed noticable brightness step while switching between almost every adjacent values, with filtered transition
* todo: *High-resolution input support*
* todo: *Device setup over DMX itself for variety of parameters*


### Brief definitions:

8bit input values are mapped to output resolution using gamma mapping, 2 by default.  
Default input is clamped at 254 - that makes it easy the remap of 7bit resolution input (0-127)
with cost of 1/256 step.

Default output resolution is 11bit depth, at maximum of 16.  
PWM frequence is in inverse ratio with resolution, being 7.8Khz at 11bit,
which is ballanced solution for smooth values switching and reduced flicker,
even when captured back on camera at relatively short shutter (1/1000).

Overall LUT function is designed to maximize utilization of lowest consecutive output values: [0,1,2,..]


todo: *`dmxx` uses 4 (default) or 7 channels per device, which are [Control, R, G, B] or [Control, R, r, G, g, B, b]*



#### Filtered transitions

Filtering is applied to make smooth output transition, mostly visible when switching
between adjacent values on high power light.

Transition time is taken so, that switching is smooth visually but no more - 
to leave fast switching of significant ranges available at the same time.



#### todo: *High-resolution input support*

Mode to allow more than 8bit input per channel.  
This gives ability to switch filtering and LUT off, to control output directly.



#### todo: *Device setup over root DMX channel*

`dmxx` root DMX channel is listened for configuration values.

Controllable settings are:

* LUT: maximum input value (254)
* LUT: PWM resolution (11 bits)
* LUT: power function (2.2), output gamma
* LUT: warmup value (1), the constant added to output,
making device being lit minimally all the time
* LUT: virtual offset, used to tune mapping of lowest values, see table
* LUT: button (1), effectively switch LUT on or off

* Filter: ratio, which directly implies smoothing gradient time

* Hires: mode (0), for 1/2 DMX channels (input bytes) per output

