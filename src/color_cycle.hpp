//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COLOR_CYCLE_HPP_
#define COLOR_CYCLE_HPP_
#include "ws2811.h"
#include <util/delay.h>

namespace color_cycle
{

template< uint16_t size>
void scroll( ws2811::rgb new_value, ws2811::rgb (&range)[size])
{
    for (uint8_t idx = size-1; idx != 0 ; --idx)
    {
        range[idx] = range[idx -1];
    }
    range[0] = new_value;
}

template< uint16_t led_count>
void animate( const ws2811::rgb &new_value, ws2811::rgb (&leds)[led_count], uint8_t channel)
{
    scroll( new_value, leds);
    send( leds, channel);
    _delay_ms( 40);
}

template<uint8_t count, uint16_t led_count>
void color_cycle( const ws2811::rgb (&sequence)[count], ws2811::rgb (&leds)[led_count], uint8_t channel)
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

}

#endif /* COLOR_CYCLE_HPP_ */
