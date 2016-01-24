//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FLARES_HPP_
#define FLARES_HPP_
//#include <stdlib.h>
#include <util/delay.h>
#include "ws2811.h"

namespace flares
{
using ws2811::rgb;

/// very crude pseudo random generator
uint16_t my_rand()
{
	static uint16_t state;
	return state += 13331; // adding a prime number
}

template<typename buffer_type, typename pos_type = uint16_t>
class flare
{
public:
	void step(buffer_type &leds)
	{
		step();
		set(leds);
	}

	flare()
	//:color( 255,255,255), position(0), amplitude(0), speed(0)
	{
	}

	rgb      color;
	pos_type position;
	pos_type amplitude;
	int8_t   speed;

private:

	/// multiply an 8-bit value with an 8.8 bit fixed point number.
	/// multiplier should not be higher than 1.00 (or 256).
	static uint8_t mult(uint8_t value, uint16_t multiplier)
	{
		return (static_cast<uint16_t>(value) * multiplier) >> 8;
	}

	rgb calculate() const
	{
		return rgb(
				mult(color.red,   amplitude),
				mult(color.green, amplitude),
				mult(color.blue,  amplitude));
	}

	void set(buffer_type &leds) const
	{
		rgb &myled = get(leds, position);
		myled = calculate();
	}

	void step()
	{
		if (speed < 0 && static_cast<uint16_t>(-speed) > amplitude)
		{
			amplitude = 0;
		}
		else
		{
			if (255 - amplitude < speed)
			{
				amplitude = 255;
				speed = -(speed / 2 + 1);
			}
			else
			{
				amplitude += speed;
			}
		}
	}

};

uint8_t random_brightness()
{
	return 170 - (my_rand() % 96);
}

template<typename flare_type>
void create_random_flare(flare_type &f, uint16_t count)
{
	f.color = rgb(random_brightness(), random_brightness(),
			random_brightness());
	f.amplitude = 0;
	f.position = (my_rand() ^ (my_rand()>>1)) % count; // not completely random.
	f.speed = (2 * (my_rand() & 0x0f)) + 5;
}

template<uint8_t flare_count, typename buffer_type>
void flares(buffer_type &leds, uint8_t channel)
{

	const uint8_t led_count = ws2811::led_buffer_traits<buffer_type>::count;

	flare<buffer_type, uint8_t> flares[flare_count];
	uint8_t current_flare = 0;
	uint8_t flare_pause = 1;

	for (;;)
	{
		if (flare_pause)
		{
			--flare_pause;
		}
		else
		{
			if (!flares[current_flare].amplitude)
			{
				create_random_flare(flares[current_flare], led_count);
				flare_pause = my_rand() % 20;
			}
			++current_flare;
		}
		if (current_flare >= flare_count)
			current_flare = 0;

		clear(leds);
		for (uint8_t idx = 0; idx < flare_count; ++idx)
		{
			flares[idx].step(leds);
		}

		send(leds, channel);
		_delay_ms(20);
	}
}
} // end namespace flares

#endif /* FLARES_HPP_ */
