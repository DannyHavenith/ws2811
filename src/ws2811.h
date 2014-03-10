//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * This header does the following things
 * - It defines the macro WS2811_PORT to PORTC if it wasn't defined yet.
 * - It includes the right version of ws2811_xx.h, depending on F_CPU
 * - It defines a convenience overload of the send()-function that auto-detects array sizes.
 */

#ifndef WS2811_H_
#define WS2811_H_
#include <string.h> // for memset

namespace ws2811 {
template<typename buffer_type>
struct led_buffer_traits
{
};
}

#if !defined( WS2811_PORT)
#	define WS2811_PORT PORTC
#endif

#if (F_CPU == 8000000)
#   include "ws2811_8.h"
#elif (F_CPU == 9600000)
#   include "ws2811_96.h"
#else
#   error "ws2811 code works with clock frequencies of 8Mhz or 9.6Mhz only."
#endif

namespace ws2811 {

/**
 * Convenience wrapper around the send() function.
 * This overload auto-detects the array size of the given rgb values.
 */
template< uint16_t array_size>
inline void send( rgb (&values)[array_size], uint8_t bit)
{
	send( &values[0], array_size, bit);
}

template< uint16_t array_size>
inline rgb& get( rgb (&values)[array_size], uint16_t index)
{
	return values[index];
}

template< uint16_t array_size>
inline void clear( rgb (&values)[array_size])
{
	memset( (void *)values, 0, sizeof values);
}

template< uint16_t array_size>
struct led_buffer_traits<rgb[array_size]>
{
	static const uint16_t count = array_size;
	static const uint16_t size = sizeof( rgb) * array_size;
};
}



#endif /* WS2811_H_ */
