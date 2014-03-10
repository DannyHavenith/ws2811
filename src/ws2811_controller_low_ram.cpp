//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * Demonstration of the ws2811 code for low-ram devices such as the attiny13
 */
#define WS2811_PORT PORTB

#include <avr/io.h>
#include "ws2811.h"
//#include "chasers.hpp"
#include "water_torture.hpp"
#include "flares.hpp"
using namespace ws2811;

namespace {
	/// transmit on bit 4
	const uint8_t channel = 4;
	const uint8_t led_string_size = 60;
	typedef sparse_leds<22, led_string_size> buffer_type;
	//typedef ws2811::rgb buffer_type[60];
	buffer_type buffer;

}

int main()
{
    DDRB = _BV(channel);

    //chasers_low_ram( buffer, channel);
    water_torture::animate( buffer, channel);
    //flares::flares<4>( buffer, channel);
}
