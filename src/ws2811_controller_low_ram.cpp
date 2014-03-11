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
#include "chasers.hpp"
using namespace ws2811;

namespace {
	/// transmit on bit 4
	const uint8_t channel = 4;
	const uint8_t led_string_size = 60;
	typedef sparse_leds<38, led_string_size> buffer_type;
	buffer_type buffer;

	typedef chaser<buffer_type, int8_t, 5> chaser_type;
	chaser_type chasers_array[] = {
			chaser_type( rgb( 50, 75, 15), 0),
			chaser_type( rgb( 50, 15, 75), -30),
//			chaser_type( rgb( 255, 0,0), 50)
	};

	inline void chasers()
	{
		clear( buffer);
		for(;;)
		{
			clear( buffer);
			for ( uint8_t idx = 0; idx < sizeof chasers_array/sizeof chasers_array[0]; ++idx)
			{
				chasers_array[idx].step( buffer);
			}
			send( buffer, channel);
			_delay_ms( 25);
		}
	}

}



int main()
{
    DDRB = _BV(channel);

    chasers();
}
