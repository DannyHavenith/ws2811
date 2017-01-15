//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * Simulation of a campfire using a WS2811 led string.
 * This code creates a campfire-like pattern by spacing 'flames' over the led string.
 * Each flame is a simple sequence that moves from dark and red-ish to light and yellow-ish and back.
 * The animation consists of letting each flame perform a random walk over the led string, adding
 * intensities where two flames overlap.
 * When the led string is curled up in a ball-shape and placed under a diffusing cover, this gives
 * a reasonably realistic fire animation.
 */

#ifndef CAMPFIRE_HPP_
#define CAMPFIRE_HPP_
#include <string.h> // for memset

#include "ws2811/rgb_operators.hpp"
#include "ws2811/ws2811.h"
namespace {
	using ws2811::rgb;
	/// flame color pattern. This pattern is twice as big as the pattern that is actually drawn to allow
	/// anti-aliasing when rendering.
	const ws2811::rgb pattern[] = {
			rgb(2,0,0), rgb(8,0,0), rgb(15, 0,0 ),
			rgb( 20, 0, 0), 	rgb( 50, 0, 0),
			rgb( 80, 0, 0), 	rgb( 100, 20, 0),
			rgb( 120, 40, 0), 	rgb( 120, 60, 5),
			rgb( 120, 80, 10), 	rgb( 170, 110, 20),
			rgb( 240, 140, 30), rgb( 170, 110, 20),
			rgb( 120, 80, 10), 	rgb( 120, 60, 5),
			rgb( 120, 40, 0), 	rgb( 100, 20, 0),
			rgb( 80, 0, 0), 	rgb( 50, 0, 0),
			rgb( 20, 0, 0),	    rgb( 15, 0, 0),
			rgb( 8, 0, 0), rgb( 2, 0,0)
	};
	const uint8_t pattern_size = sizeof pattern/sizeof pattern[0];
}

/// A flame renders a flame-colored pattern on a sequence of leds.
/// At each step of the animation, the flame will make a move to the left or
/// right at random.
/// A flame will remember its position. Note that the position is stored as
/// a 15.1 fixed point number and that the used color sequence is also twice as
/// big as the sequence that is actually drawn. This is done to create a smoother
/// animation.
class flame
{
public:
	explicit flame( uint16_t position = 0)
	:position( position * 2){}

	void step( rgb *leds, uint16_t size)
	{
		if (((rand()&7)==0) && position > 0)
		{
			position--;
		}
		if (((rand()&7)==0) && ((position + pattern_size) < size*2))
		{
			position++;
		}

		draw( leds, size);
	}

private:
	uint16_t position;

	/// Draw the color sequence at this flames position.
	void draw(rgb* leds, uint16_t size)
	{
		for (uint8_t color = 0; color < sizeof pattern / sizeof pattern[0];
				++color)
		{
			// note that we're only using half of the sequence colors
			if (!((position + color)&1) && (position + color)/2 < size)
			{
				add_clipped( leds[(position + color)/2], pattern[color]);
			}
		}
	}
};

/// Animate a campfire on a WS2811 led string.
/// This function creates a fixed amount of flame objects, disperses
/// them over the led string and then lets the flames do their animation in
/// a infinite loop.
template< uint16_t size>
void campfire( rgb (&leds)[size], uint8_t channel)
{
	static const uint8_t flamecount = size/10;
	flame flames[flamecount];
	const uint16_t step = (size - pattern_size)/flamecount;
	for (uint16_t pos = 0; pos < flamecount; ++pos)
	{
		flames[pos] = flame( step*pos);
	}
	for(;;)
	{
		clear(leds);
		for (uint8_t f = 0; f < flamecount; ++f)
		{
			flames[f].step( leds, size);
		}
		_delay_ms(20);
		send( leds, channel);
	}
}


#endif /* CAMPFIRE_HPP_ */
