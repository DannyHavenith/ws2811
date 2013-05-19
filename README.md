WS2811
======

AVR code to communicate with a WS2811 led controller from an AVR clocked from its internal 8Mhz oscillator. 
On a microcontroller like an atmega88 can control a 800kbit WS2811 led string without any additional components.
The interesting stuff is in [ws2811_8.h](src/ws2811_8.h), while the assembly code design is 
documented in [a spreadsheet](design/ws2811@8Mhz.ods?raw=true). 

A more elaborate description can be found 
[here](http://rurandom.org/justintime/index.php?title=Driving_the_WS2811_at_800_kHz_with_an_8_MHz_AVR),
while an in-depth description of the assembly code can be found 
[on this page](http://rurandom.org/justintime/index.php?title=Ws2811_driver_code_explained).

