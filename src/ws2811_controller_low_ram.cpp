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
#include <util/delay.h>

using namespace ws2811;

namespace {
	/// transmit on bit 4
	const uint8_t channel = 4;
	uint8_t buffer[30];
}

void write_to( uint8_t *position, const rgb &color)
{
	*position++ = color.green;
	*position++ = color.red;
	*position++ = color.blue;
}

int main()
{
    DDRB = _BV(channel);
    for(;;)
    {
		for (uint8_t count = 0; count < 19; ++count)
		{
			buffer[0] = count;
			write_to( &buffer[1], rgb( 255, 0, 255));
			send_sparse( buffer, channel);
			_delay_ms(10);
		}
		for (uint8_t count = 20; count ; --count)
		{
			buffer[0] = count;
			write_to( &buffer[1], rgb( 255, 0, 255));
			send_sparse( buffer, channel);
			_delay_ms(10);
		}
    }
}
