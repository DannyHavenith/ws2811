//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "chasers.hpp"
#include "flares.hpp"
//#include "color_cycle.hpp"

int main()
{
	static const uint8_t channel = 5;
    DDRC = _BV(channel);
    flares::flares( channel);
//	chasers( channel);
    //color_cycle::example_color_cycle( 5);
}
