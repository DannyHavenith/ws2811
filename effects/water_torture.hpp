/*
 * water_torture.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: danny
 */

#ifndef WATER_TORTURE_HPP_
#define WATER_TORTURE_HPP_
#include <util/delay.h>
#include "ws2811/ws2811.h"

namespace water_torture
{
	using ws2811::rgb;
	
	/// very crude pseudo random generator.
	///
	/// The idea is to have a good enough random generator
	/// with a minimum of program space.
	uint16_t my_rand()
	{
		static uint16_t state;
		uint8_t count = 1+ state %5;
		state += count *33203;
		return state; 
	}

	uint8_t mult( uint8_t value, uint16_t multiplier)
	{
		return (static_cast<uint16_t>( value) * multiplier) >> 8;
	}

	/// This class maintains the state and calculates the animations to render a falling water droplet
	/// Objects of this class can have three states:
	///    - inactive: this object does nothing
	///    - swelling: the droplet is at the top of the led strip and swells in intensity
	///    - falling: the droplet falls downwards and accelerates
	///    - bouncing: the droplet has bounced of the ground. A smaller, less intensive droplet bounces up
	///      while a part of the drop remains on the ground.
	/// After going through the swelling, falling and bouncing phases, the droplet automatically returns to the
	/// inactive state.
	template<typename buffer_type, bool allow_swelling = true>
	class droplet
	{
	public:
		droplet( const rgb &color)
		:color( color), position(0), speed(0),state(allow_swelling?swelling:falling)
		{}

		droplet() // by default, be uninitialized
		{

		}
		/// calculate the next step in the animation for this droplet
		void step()
		{
			static uint8_t maxpos =
					ws2811::led_buffer_traits<buffer_type>::count - 1;
			if (state == falling || state == bouncing)
			{
				position += speed;
				speed += gravity;

				// if we hit the bottom...
				const uint16_t maxpos16 = maxpos << 8;
				if (position > maxpos16)
				{
					if (state == bouncing)
					{
						// this is the second collision,
						// deactivate.
						state = inactive;
					}
					else
					{
						// reverse direction and dampen the speed
						position = maxpos16 - (position - maxpos16);
						speed = -speed/3;
						color = scale( color, 10);
						state = bouncing;
					}
				}
			}
			else if (allow_swelling && state == swelling)
			{
				++position;
				if ( color.blue <= 10 || color.blue - position/2 <= 10)
				{
					state = falling;
					position = 0;
				}

			}
		}

		/// perform one step and draw.
		void step( buffer_type &leds)
		{
			step();
			draw( leds);
		}

		/// Draw the droplet on the led string
		/// This will "smear" the light of this droplet between two leds. The closer
		/// the droplets position is to that of a particular led, the brighter that
		/// led will be
		void draw( buffer_type &leds)
		{
			static uint8_t max_pos =
					ws2811::led_buffer_traits<buffer_type>::count - 1;
			if (state == falling || state == bouncing)
			{
				uint8_t position8 = position >> 8;
				uint8_t remainder = position; // get the lower bits

				add_clipped_to( get( leds, position8), scale( color, 256 - remainder ));
				if (remainder)
				{
					add_clipped_to( get( leds, position8+1), scale( color, remainder));
				}

				if (state == bouncing)
				{
					add_clipped_to( get( leds, max_pos), color);
				}
			}
			else if (allow_swelling && state == swelling)
			{
				add_clipped_to( get( leds, 0), scale( color, position));
			}
		}

		bool is_active() const
		{
			return state != inactive;
		}

private:
		/// Add  two numbers and clip the result at 255.
		static uint8_t add_clipped( uint16_t left, uint16_t right)
		{
			uint16_t result = left + right;
			if (result > 255) result = 255;
			return result;
		}

		/// Add the right rgb value to the left one, clipping if necessary
		static void add_clipped_to( rgb &left, const rgb &right)
		{
					left.red   = add_clipped(left.red, right.red);
					left.green = add_clipped( left.green, right.green);
					left.blue  = add_clipped( left.blue, right.blue);
		}

		/// multiply an 8-bit value with an 8.8 bit fixed point number.
		/// multiplier should not be higher than 1.00 (or 256).
		static uint8_t mult( uint8_t value, uint16_t multiplier)
		{
			return (static_cast<uint16_t>( value) * multiplier) >> 8;
		}

		/// scale an rgb value up or down. amplitude > 256 means scaling up, while
		/// amplitude < 256 means scaling down.
		static  rgb scale(rgb value, uint16_t amplitude)
		{
			return rgb(
					mult( value.red, amplitude),
					mult( value.green, amplitude),
					mult( value.blue, amplitude)
			);
		}

		// how much of a color is left when colliding with the floor, value
		// between 0 and 256 where 256 means no loss.
		static const uint16_t collision_scaling = 40;
		rgb 	 color;
		uint16_t position; //< position in 8.8 fixed point, 256 means at LED 1, 384 means between LEDS 1 and 2, etc.
		int16_t  speed;    //< speed in 8.8 fixed point, 256 means 1 LED per cycle, 512 means 2 LEDs/cycle, etc.
		static const uint16_t gravity = 8;
		enum stateval {
			inactive,
			swelling,
			falling,
			bouncing
		};

		stateval state;
	};

	uint8_t debugcount = 0;
	volatile uint16_t random_scale()
	{
		return (my_rand() % 256);
	}

	template< typename buffer_type, bool allow_swelling>
	void create_random_droplet( droplet<buffer_type, allow_swelling> &d)
	{
		d = droplet<buffer_type, allow_swelling>(
				rgb(
						mult( 100 ,random_scale()),
						mult( 100, random_scale()),
						mult(255, random_scale())
						));
	}

	template<bool assertion>
	struct static_assert_ {};

	template<>
	struct static_assert_<true>
	{
	  static void is_true(){};
	};

	/// Create the complete water torture animation.
	/// This will render droplets at random intervals, up to a given maximum number of droplets.
	/// The maximum led count is 256
	template< uint8_t droplet_count, typename buffer_type, typename transmitter_type>
	void inline animate( buffer_type &leds, const transmitter_type &transmitter)
	{
		static const uint16_t led_count = ws2811::led_buffer_traits<buffer_type>::count;

	    // if you get an error that 'is_true' is not a member of static_assert_, you're probably using
	    // more than 255 leds, which doesn't work for this function.
	    static_assert_< led_count <= 255>::is_true();

	    typedef droplet<buffer_type, true> droplet_type;
	    droplet_type droplets[droplet_count]; // droplets that can animate simultaneously.
	    uint8_t current_droplet = 0; // index of the next droplet to be created
	    uint16_t droplet_pause = 1; // how long to wait for the next one

	    for(;;)
	    {
	    	if (droplet_pause)
	    	{
	    		--droplet_pause;
	    	}
	    	else
	    	{
	    		if (!droplets[current_droplet].is_active())
	    		{
	    			create_random_droplet( droplets[current_droplet]);
	    			++current_droplet;
	    			if (current_droplet >= droplet_count) current_droplet = 0;
	    			droplet_pause = 200 + my_rand() % 128;
	    		}
	    	}

	    	clear( leds);
	    	for (uint8_t idx = 0; idx < droplet_count; ++idx)
	    	{
	    		droplets[idx].step( leds);
	    	}

	    	transmitter.transmit( leds);
	    	_delay_ms( 1);
	    }
	}
}


#endif /* WATER_TORTURE_HPP_ */
