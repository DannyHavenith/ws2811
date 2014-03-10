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

#define WS2811_PORT PORTB

#include "chasers.hpp"
//#include "flares.hpp"
//#include "color_cycle.hpp"
//#include "water_torture.hpp"
//#include "campfire.hpp"
namespace {

static const uint8_t channel = 5;
static const uint16_t led_count = 60;

ws2811::rgb leds[led_count];

}
int main()
{
    DDRB = _BV(channel);
    //campfire( leds, channel);
    //water_torture::animate( leds, channel);
    //flares::flares( channel);
    chasers( channel);
    //color_cycle::example_color_cycle( 5);
}
