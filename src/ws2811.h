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
void send( rgb (&values)[array_size], uint8_t bit)
{
	send( &values[0], array_size, bit);
}

}



#endif /* WS2811_H_ */
