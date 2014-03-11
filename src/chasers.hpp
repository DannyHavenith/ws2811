//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHASERS_HPP_
#define CHASERS_HPP_
#include <stdlib.h>
#include <util/delay.h>
#include "ws2811.h"
using ws2811::rgb;

namespace
{
}

template<typename buffer_type, typename pos_type = int16_t, uint8_t tail_count = 16>
class chaser
{
public:
	void step( buffer_type &leds)
	{

		step();
		draw( leds);
	}

	chaser( const rgb &color, pos_type position)
	:color( color), position(position)
	{}

	rgb 	 color;
	pos_type position;

private:


	/// multiply an 8-bit value with an 8.8 bit fixed point number.
	/// multiplier should not be higher than 1.00 (or 256).
	static uint8_t mult( uint8_t value, uint16_t multiplier)
	{
		return (static_cast<uint16_t>( value) * multiplier) >> 8;
	}

	static rgb scale(rgb value, uint16_t amplitude)
	{
		return rgb(
				mult( value.red, amplitude),
				mult( value.green, amplitude),
				mult( value.blue, amplitude)
		);
	}

	static uint8_t add_clipped( uint8_t left, uint8_t right)
	{
		uint16_t result = static_cast<uint16_t>(left) + right;
		if (result > 255) result = 255;
		return result;
	}

	static rgb add_clipped( const rgb &left, const rgb &right)
	{
		return rgb(
				add_clipped(left.red, right.red),
				add_clipped( left.green, right.green),
				add_clipped( left.blue, right.blue)
				);
	}

	static pos_type abs( pos_type pos)
	{
		return (pos < 0)?-pos:pos;
	}

	void draw( buffer_type &leds) const
	{
		static const uint8_t size = ws2811::led_buffer_traits<buffer_type>::count;
		pos_type pos = position;
		uint16_t amplitude = 256-256/tail_count + 4;
		for (uint8_t count = tail_count; count ; --count)
		{
			rgb &loc = get(leds, abs( pos));
			loc = add_clipped( loc, scale( color, amplitude));
			amplitude -= 256/tail_count; // amplitude *= 0.75
			--pos;
			if( pos == -size)
			{
				pos = size -1;
			}
		}
	}

	void step( )
	{
		static const uint8_t size = ws2811::led_buffer_traits<buffer_type>::count;
		if (++position >= size)
		{
			position = -(size-1);
		}

	}

};

template<typename buffer_type>
inline void chasers( buffer_type &leds, uint8_t channel)
{

	typedef chaser<buffer_type> chaser_type;
	chaser_type chasers[] = {
			chaser_type( rgb( 50, 75, 15), 0),
			chaser_type( rgb( 10, 40, 60), 30),
			chaser_type( rgb( 255, 0,0), 50),
			chaser_type( rgb( 100, 100, 100), -35)
	};

	for(;;)
	{
		clear( leds);
		for ( uint8_t idx = 0; idx < sizeof chasers/sizeof chasers[0]; ++idx)
		{
			chasers[idx].step( leds);
		}
		send( leds, channel);
		_delay_ms( 30);
	}
}


#endif /* CHASERS_HPP_ */
