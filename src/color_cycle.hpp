//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COLOR_CYCLE_HPP_
#define COLOR_CYCLE_HPP_
#include "ws2811_8.h"

namespace color_cycle
{

template< uint8_t size>
void scroll( ws2811::rgb new_value, ws2811::rgb (&range)[size])
{
    for (uint8_t idx = size-1; idx != 0 ; --idx)
    {
        range[idx] = range[idx -1];
    }
    range[0] = new_value;
}

template< uint8_t led_count>
void animate( const ws2811::rgb &new_value, ws2811::rgb (&leds)[led_count], uint8_t channel)
{
    scroll( new_value, leds);
    send( leds, channel);
    _delay_ms( 40);
}

template<uint8_t count, uint8_t led_count>
void color_cycle( ws2811::rgb (&sequence)[count], ws2811::rgb (&leds)[led_count], uint8_t channel)
{
	for (;;)
	{
		for (uint8_t idx = 0; idx != count; ++idx)
		{
			animate( sequence[idx], leds, channel);
		}
		for (uint8_t idx = count; idx != 0; --idx)
		{
			animate( sequence[idx-1], leds, channel);
		}

	}
}

void example_color_cycle( uint8_t channel)
{
	using ws2811::rgb;
	rgb leds[60];
	rgb sequence[] = {
			rgb( 0, 10, 20),
			rgb( 0, 20, 40),
			rgb( 0, 30, 60),
			rgb( 10, 30, 40),
			rgb( 20, 40, 50),
			rgb( 30, 50, 40),
			rgb( 40, 50, 40),
			rgb( 50, 50, 40),
			rgb( 60, 50, 40),
			rgb( 100, 50, 40),
			rgb( 100, 40, 30),
			rgb( 100, 30, 20),
			rgb( 100, 20, 10),
			rgb( 100, 0,0)
	};

	color_cycle( sequence, leds, channel);

}
}

#endif /* COLOR_CYCLE_HPP_ */
