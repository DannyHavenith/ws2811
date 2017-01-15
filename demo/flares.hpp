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

#include "ws2811/ws2811.h"

namespace flares
{
using ws2811::rgb;

/// very crude pseudo random generator
uint16_t my_rand()
{
	static uint16_t state;
	return state += 13331; // adding a prime number
}

const rgb base_color( 3, 2, 0);

template <typename T>
T max( T lhs, T rhs)
{
    return (lhs > rhs) ? lhs : rhs;
}

template<typename buffer_type, typename pos_type = uint16_t>
class flare
{
public:
    /**
     * If directionFilter is negative, then flares will only be written to the LED string
     * when dimming, if directionFilter is positive then only growing flares will be written.
     * If directionFilter is zero, the lights will be written both when growing and when
     * dimming.
     */
	void step(buffer_type &leds, int8_t directionFilter = 0)
	{
		step();
		set(leds, directionFilter);
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
				base_color.red   + mult(color.red,   amplitude),
				base_color.green + mult(color.green, amplitude),
				base_color.blue  + mult(color.blue,  amplitude));
	}

	void set(buffer_type &leds, int8_t directionFilter) const
	{
	    if (speed * directionFilter >= 0)
	    {
            rgb &myled = get(leds, position);
            myled = calculate();
	    }
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
				speed = -(speed);
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

template<typename flare_type, typename buffer_type>
void create_random_flare(flare_type &f, buffer_type &leds, bool onlyZeroLeds = true)
{
    const uint8_t count = ws2811::led_buffer_traits<buffer_type>::count;
	f.color = rgb(random_brightness() +10, random_brightness() / 2 - 10,
			random_brightness()/3);
	f.amplitude = 0;
	f.speed = 0;
	uint8_t tryLeds = 100;
	while (tryLeds--)
	{
        f.position = (my_rand() ^ (my_rand()>>1)) % count; // not completely random.
	    if (   (    onlyZeroLeds and get(leds, f.position) == base_color)
	        or (not onlyZeroLeds and get(leds, f.position) != base_color))
	    {
	        tryLeds = 0;
	        f.speed = ( (my_rand() & 0x07)) + 1;
	    }
	}

}

template<uint8_t flare_count, typename buffer_type>
void flares(buffer_type &leds, uint8_t channel, uint16_t duration, int8_t direction = 0)
{

	flare<buffer_type, uint8_t> flares[flare_count];
	uint8_t current_flare = 0;
	uint8_t flare_pause = 1;

	for (;duration;--duration)
	{
		if (flare_pause)
		{
			--flare_pause;
		}
		else
		{
			if (!flares[current_flare].amplitude)
			{
				create_random_flare(flares[current_flare], leds, direction >= 0);
				flare_pause = 0;//my_rand() % 11;
			}
			++current_flare;
		}
		if (current_flare >= flare_count)
			current_flare = 0;

		for (uint8_t idx = 0; idx < flare_count; ++idx)
		{
			flares[idx].step(leds, direction);
		}

		send(leds, channel);
		_delay_ms(30);
	}
}

template<uint8_t flare_count, typename buffer_type>
void flares(buffer_type &leds, uint8_t channel)
{
    DDRD = 255;
    for (uint8_t counter = 0; counter < ws2811::led_buffer_traits<buffer_type>::count; ++counter)
    {
        get( leds, counter) = base_color;
    }
    for (;;)
    {
        flares<flare_count>( leds, channel, 65535, 0);
    }
}

} // end namespace flares

#endif /* FLARES_HPP_ */
