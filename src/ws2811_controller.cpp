//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define WS2811_PORT PORTB

#include "chasers.hpp"
//#include "flares.hpp"
//#include "color_cycle.hpp"
//#include "water_torture.hpp"
//#include "campfire.hpp"

int main()
{
    static const uint8_t channel = 4;
    //static const uint16_t led_count = 60;
    DDRB = _BV(channel);
    //ws2811::rgb leds[led_count];
    //campfire( leds, channel);
    //water_torture::animate( leds, channel);
    //flares::flares( channel);
    chasers( channel);
    //color_cycle::example_color_cycle( 5);
}
