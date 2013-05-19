//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FLARES_HPP_
#define FLARES_HPP_
#include <stdlib.h>
#include "ws2811_8.h"

namespace flares
{
using ws2811::rgb;

class flare
{
public:
	void step( rgb *leds)
	{
		step();
		set( leds);
	}

	flare()
	:color( 255,255,255), position(0), amplitude(0), speed(0)
	{}

	rgb 	 color;
	uint16_t position;
	uint16_t amplitude;
	int8_t 	 speed;

private:

	/// multiply an 8-bit value with an 8.8 bit fixed point number.
	/// multiplier should not be higher than 1.00 (or 256).
	static uint8_t mult( uint8_t value, uint16_t multiplier)
	{
		return (static_cast<uint16_t>( value) * multiplier) >> 8;
	}

	rgb calculate() const
	{
		return rgb(
				mult( color.red, amplitude),
				mult( color.green, amplitude),
				mult( color.blue, amplitude)
		);
	}

	void set( rgb *leds) const
	{
		rgb *myled = leds + position;
		*myled = calculate();
	}

	void step()
	{
		if (speed < 0 && static_cast<uint16_t>(-speed) > amplitude)
		{
			amplitude = 0;
		}
		else
		{
			amplitude += speed;
			if (amplitude > 256)
			{
				amplitude = 256;
				speed = -(speed/4 + 1);
			}
		}
	}

};

uint8_t random_brightness()
{
	return  150 - (rand() % 80);
}

void create_random_flare( flare &f, uint16_t count)
{
	f.color = rgb( random_brightness(), random_brightness(), random_brightness());
	f.amplitude = 0;
	f.position = rand() % count; // not completely random.
	f.speed = (2 * (rand() & 0x07))+4;
}

void flares( uint8_t channel)
{

    const uint8_t led_count = 60;
    rgb leds[led_count];

    const uint8_t flare_count = 16;
    flare flares[flare_count];
    uint8_t current_flare = 0;
    uint8_t flare_pause = 1;

    for(;;)
    {
    	if (flare_pause)
    	{
    		--flare_pause;
    	}
    	else
    	{
    		if (!flares[current_flare].amplitude)
    		{
    			create_random_flare( flares[current_flare], led_count);
    			++current_flare;
    			if (current_flare >= flare_count) current_flare = 0;
    			flare_pause = rand() % 80;
    		}
    	}

    	for (uint8_t idx = 0; idx < flare_count; ++idx)
    	{
    		flares[idx].step( leds);
    	}

    	send( leds, channel);
    	_delay_ms( 10);
    }
}
} // end namespace flares








#endif /* FLARES_HPP_ */
