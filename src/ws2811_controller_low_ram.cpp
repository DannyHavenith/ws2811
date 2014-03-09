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

//	void test_memmove()
//	{
//		//get(buffer, 54) = rgb( 128, 128, 128);
//		uint8_t index = 0;
//		buffer.buffer[index++] = 54;
//		buffer.buffer[index++] = 1;
//		buffer.buffer[index++] = 99;
//		buffer.buffer[index++] = 99;
//		buffer.buffer[index++] = 99;
//		buffer.buffer[index++] = 0;
//		ws2811::my_memmove(&buffer.buffer[0], &buffer.buffer[55],
//				&buffer.buffer[60]);
//		index = 0;
//		buffer.buffer[index++] = 2;
//		buffer.buffer[index++] = 1;
//		buffer.buffer[index++] = 0;
//		buffer.buffer[index++] = 0;
//		buffer.buffer[index++] = 255;
//	}
}



int main()
{
    DDRB = _BV(channel);
//	clear( buffer);
//	get(buffer, 59) = rgb( 55, 0, 0);
//	get(buffer, 10) = rgb( 0, 55, 0);
//	send(buffer, channel);
//	clear( buffer);
//	buffer.buffer[0] = 59;
//	uint8_t index = 20;
//	get(buffer, index++) = rgb( 0, 0, 55);
//	get(buffer, index++) = rgb( 0, 55, 55);
//	get(buffer, index) = rgb(55, 0, 55);
//	get(buffer, index--) = rgb(55, 55, 55);
//	get(buffer, index--) = rgb(55, 55, 55);
//	get(buffer, index--) = rgb(55, 55, 55);
//	get(buffer, index--) = rgb(55, 0,0);
//
//	index = 30;
//	get(buffer, index++) = rgb( 0, 55, 55);
//
//
//
////	get(buffer, index++) = rgb(10, 10, 10);
////	get(buffer, index++) = rgb(10, 10, 10);
////	get(buffer, index++) = rgb(10, 10, 10);
//
//	for (;;) send(buffer, channel);

    chasers();
}
