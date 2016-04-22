WS2811
======

AVR code to communicate with a WS2811 led controller from an AVR clocked from its internal 8Mhz or 9.6Mhz oscillator. 
A microcontroller like an 8Mhz atmega88, attiny13 or attiny2313 using this code can control a 800kbit WS2811 led string without 
any additional components. The interesting stuff is in [ws2811_8.h](ws2811/ws2811_8.h), while the assembly 
code design is documented in [a spreadsheet](design/ws2811@8Mhz.ods?raw=true). 

A more elaborate description can be found 
[here](http://rurandom.org/justintime/index.php?title=Driving_the_WS2811_at_800_kHz_with_an_8_MHz_AVR).

This software is released under the [Boost Software License v1.0](https://github.com/DannyHavenith/ws2811/blob/master/LICENSE_1_0.txt).
