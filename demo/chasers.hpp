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
#include <avr/pgmspace.h>

#include "ws2811/ws2811.h"
using ws2811::rgb;

namespace
{

static const uint8_t amplitudes[] PROGMEM = {
		255, 200, 150, 100, 80, 60, 50, 40, 30, 20, 10, 5, 4, 3, 2, 1
};
}

/**
 * Larson scanner.
 *
 * An object of this class will move a string of leds from left-to-right
 * and from right-to-left over a WS2811 LED string. Each instantiation
 * of this class can have its own color. The first led of the string
 * will have that color, while the following leds have diminishing
 * intensities.
 *
 * When the step() or draw() functions are called, the object will
 * _add_ itself to the led string, so that overlapping chasers will
 * mix their colors.
 */
template<typename buffer_type, typename pos_type = int16_t, uint8_t tail_count = 16>
class chaser
{
public:

	/**
	 * calculate one animation step and draw the result to the
	 * LED string.
	 */
	void step( buffer_type &leds)
	{

		step();
		draw( leds);
	}

	/**
	 * Only draw the current state to the given led string, don't animate.
	 */
	void draw( buffer_type &leds) const
	{
		static const uint8_t size = ws2811::led_buffer_traits<buffer_type>::count;
		static const uint8_t amplitude_count = sizeof amplitudes/sizeof amplitudes[0];
		pos_type pos = position;
		uint16_t accumulator = 0;
		while (accumulator/tail_count < amplitude_count)
		{
			rgb &loc = get(leds, abs( pos));
			loc = add_clipped( loc, scale( color,  pgm_read_byte(&amplitudes[accumulator/tail_count])));
			accumulator += amplitude_count;
			--pos;
			if( pos == -size)
			{
				pos = size -1;
			}
		}
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

	/// scale a color with a 8.8 fixed point constant.
	/// amplitude 256 corresponds with 1.0.
	static rgb scale(rgb value, uint16_t amplitude)
	{
		return rgb(
				mult( value.red, amplitude),
				mult( value.green, amplitude),
				mult( value.blue, amplitude)
		);
	}

	/// add two 8-bit integers without overflowing. If the
	/// result of the addition is greater than 255, the result
	/// will be clipped to 255.
	static uint8_t add_clipped( uint8_t left, uint8_t right)
	{
		uint16_t result = static_cast<uint16_t>(left) + right;
		if (result > 255) result = 255;
		return result;
	}

	/// add two rgb values without overflow.
	static rgb add_clipped( const rgb &left, const rgb &right)
	{
		return rgb(
				add_clipped(left.red, right.red),
				add_clipped( left.green, right.green),
				add_clipped( left.blue, right.blue)
				);
	}

	/// return the absolute value of the given position.
	static pos_type abs( pos_type pos)
	{
		return (pos < 0)?-pos:pos;
	}


	/// Make one animation step.
	void step( )
	{
		static const uint8_t size = ws2811::led_buffer_traits<buffer_type>::count;
		if (++position >= size)
		{
			position = -(size-1);
		}

	}

};

template<typename buffer_type, typename chaser_array>
inline void chasers( buffer_type &buffer, chaser_array &chasers_array, uint8_t channel)
{
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

template<typename buffer_type>
inline void chasers( buffer_type &leds, uint8_t channel)
{

	typedef chaser<buffer_type> chaser_type;

	chaser_type chasers_array[] = {
			chaser_type( rgb( 50, 75, 15), 0),
			chaser_type( rgb( 10, 40, 60), 30),
			chaser_type( rgb( 255, 0,0), 50),
			chaser_type( rgb( 100, 100, 100), -35)
	};

	chasers( leds, chasers_array, channel);
}

template< typename buffer_type>
inline void chasers_low_ram( buffer_type &buffer, uint8_t channel)
{
	typedef chaser<buffer_type, int8_t, 5> chaser_type;
	chaser_type chasers_array[] = {
			chaser_type( rgb( 50, 75, 15), 0),
			chaser_type( rgb( 50, 15, 75), -30)
	};

	chasers( buffer, chasers_array, channel);
}

#endif /* CHASERS_HPP_ */
