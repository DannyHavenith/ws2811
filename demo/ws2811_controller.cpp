//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * Demonstration of the ws2811 code for devices with enough RAM to hold values for 60 LEDs.
 *
 * Depending on which effect you want and on which port/bit you want to transmit do the following
 * - un-comment the appropriate include file
 * - define WS2811_PORT to be the port that you want to transmit on
 * - define the variable 'channel' as the bit you want to use for transmission
 * - make sure that you initialize the DDRn register, so that bit 'channel' becomes an output.
 * - call the appropriate code from main.
 *
 * Note that some demo functions still declare their own array of LEDs, and some require the main
 * function to declare such an array.
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// send RGB in R,G,B order instead of the standard WS2811 G,R,B order.
// Most ws2811 LED strips take their colors in GRB order, while some LED strings
// take them in RGB. Default is GRB, define this symbol for RGB.
#define STRAIGHT_RGB
#include "ws2811/ws2811.h"
//#include "effects/chasers.hpp"
#include "effects/flares.hpp"
//#include "effects/color_cycle.hpp"
//#include "effects/water_torture.hpp"
//#include "effects/campfire.hpp"

namespace {

// declare one RGB value for each led.
ws2811::rgb leds[144];

// transmit at port C4
static const uint8_t    channel = 4;
ws2811::Transmitter< ws2811::AtPortC> transmitter( channel);

}

int main()
{
    DDRC = _BV(channel);


    //campfire( leds, channel);
    //water_torture::animate<3>( leds, channel);
    flares::flares<20>( leds, transmitter);
    //chasers( leds, channel);
    //color_cycle::color_cycle(pattern, leds, channel);
//    send( leds, channel);
//    send( leds, channel);
}
