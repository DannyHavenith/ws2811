//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "chasers.hpp"
//#include "flares.hpp"
//#include "color_cycle.hpp"
#include "water_torture.hpp"

int main()
{
	static const uint8_t channel = 5;
	static const uint8_t led_count = 60;
    DDRC = _BV(channel);
    ws2811::rgb leds[led_count];
    water_torture::animate( leds, led_count, channel);
//    flares::flares( channel);
	//chasers( channel);
    //color_cycle::example_color_cycle( 5);
}
