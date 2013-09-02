//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * Operators for RGB values.
 * Specifically, this file contains functions for "clipped addition" where the values of two bytes that are added
 * will never overflow, but will instead be clipped at the maximum value of 255.
 */

#ifndef RGB_OPERATORS_HPP_
#define RGB_OPERATORS_HPP_
#include "rgb.h"

namespace ws2811
{
	namespace detail {
		inline uint8_t add_clipped( uint16_t left, uint16_t right)
		{
			uint16_t result = left + right;
			if (result > 255) result = 255;
			return result;
		}
	}

	inline void add_clipped( rgb &left, const rgb &right)
	{
		using detail::add_clipped;
		left = rgb(
				add_clipped(left.red, right.red),
				add_clipped( left.green, right.green),
				add_clipped( left.blue, right.blue)
				);
	}

}



#endif /* RGB_OPERATORS_HPP_ */
