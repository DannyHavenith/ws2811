//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * Library for bit-banging data to WS2811 led controllers.
 * This file contains a definition of the rgb and the send() function.
 */

#ifndef WS2811_8_H_
#define WS2811_8_H_
#include <avr/io.h>

#if !defined( WS2811_PORT)
#	define WS2811_PORT PORTC
#endif


namespace ws2811
{
/**
 * Type that holds RGB values.
 * The in-memory order of this type is actually GRB, but the constructor takes
 * its values in RGB order.
 */
struct rgb
{
    rgb(uint8_t red, uint8_t green, uint8_t blue)
    :green(green),red(red),blue(blue)
    {}

    rgb()
    :green(0),red(0),blue(0)
    {}

    uint8_t green;
    uint8_t red;
    uint8_t blue;
};


/**
 * This function sends the RGB-data in an array of rgb structs through
 * the given io-pin.
 * The port is determined by the macro WS2811_PORT, but the actual pin to
 * be used is an argument to this function. This allows a single instance of this function
 * to control up to 8 separate channels.
 */

void send( const rgb *values, uint8_t array_size, uint8_t bit)
{
    const uint8_t mask =_BV(bit);
    uint8_t low_val = WS2811_PORT & (~mask);
    uint8_t high_val = WS2811_PORT | mask;
    uint8_t size = array_size * sizeof values[0]; // size in bytes


    // reset the controllers by pulling the data line low
    uint8_t bitcount = 4;
    WS2811_PORT = low_val;
    _delay_us( 40);

    // note: the labels in this piece of assembly code aren't very explanatory. The real documentation
    // of this code can be found in the spreadsheet ws2811@8Mhz.ods
    // The order of the blocks of code have been determined by arrange_timed_code.cpp
    // The code could be made a little shorter by combining adjacent nops into RJMPS, but since I'm expecting
    // to re-assemble the code fragments from the spreadsheet at some point, I'm not bothering with that now.
    asm volatile(
"            LD __tmp_reg__, %a[dataptr]+"	"\n" // fill the first byte and determine first bit value
"            LSL __tmp_reg__"	"\n"
"            LDI %[bits], 4"    "\n"
"            BRCC L13"	"\n"
"            RJMP L5"	"\n"
"    H1:     NOP"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            NOP"	"\n"
"            RJMP END"	"\n"
"    L1:     SUBI %[bits], 1"	"\n"
"            BREQ M2"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            LSL __tmp_reg__"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            BRCC L8"	"\n"
"            NOP"	"\n"
"    L15:    NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            RJMP L9"	"\n"
"    M4:     LDI %[bits], 4"	"\n"
"            LD __tmp_reg__, %a[dataptr]+"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            SUBI %[bytes], 1"	"\n"
"            BREQ H1"	"\n"
"            LSL __tmp_reg__"	"\n"
"            BRCS L9"	"\n"
"            NOP"	"\n"
"    L16:    OUT %[portout], %[downreg]"	"\n"
"            NOP"	"\n"
"    L13:    OUT %[portout], %[upreg]"	"\n"
"            LSL __tmp_reg__"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            BRCC L10"	"\n"
"            SUBI %[bits], 1"	"\n"
"            BREQ M4"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            LSL __tmp_reg__"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            BRCS L15"	"\n"
"            NOP"	"\n"
"    L8:     NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            RJMP L16"	"\n"
"    M2:     LDI %[bits], 4"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            LD __tmp_reg__, %a[dataptr]+"	"\n"
"            SUBI %[bytes], 1"	"\n"
"            BREQ H1"	"\n"
"            LSL __tmp_reg__"	"\n"
"            BRCC L16"	"\n"
"            NOP"	"\n"
"    L9:     OUT %[portout], %[downreg]"	"\n"
"            NOP"	"\n"
"    L5:     OUT %[portout], %[upreg]"	"\n"
"            LSL __tmp_reg__"	"\n"
"            BRCS L1"	"\n"
"            SUBI %[bits], 1"	"\n"
"            BREQ M1"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            LSL __tmp_reg__"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            BRCC L4"	"\n"
"            NOP"	"\n"
"    L12:    NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            RJMP L5"	"\n"
"    M3:     LD __tmp_reg__, %a[dataptr]+"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            LDI %[bits], 4"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            SUBI %[bytes], 1"	"\n"
"            BREQ H2"	"\n"
"            LSL __tmp_reg__"	"\n"
"            BRCC L16"	"\n"
"            NOP"	"\n"
"            RJMP L5"	"\n"
"    L10:    SUBI %[bits], 1"	"\n"
"            BREQ M3"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            LSL __tmp_reg__"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            BRCS L12"	"\n"
"            NOP"	"\n"
"    L4:     NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            RJMP L13"	"\n"
"    M1:     LD __tmp_reg__, %a[dataptr]+"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            LDI %[bits], 4"	"\n"
"            OUT %[portout], %[upreg]"	"\n"
"            NOP"	"\n"
"            OUT %[portout], %[downreg]"	"\n"
"            SUBI %[bytes], 1"	"\n"
"            BREQ H2"	"\n"
"            LSL __tmp_reg__"	"\n"
"            BRCS L9"	"\n"
"            NOP"	"\n"
"            RJMP L13"	"\n"
"    H2:     NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"            NOP"	"\n"
"    END:    NOP"	"\n"
: /* no output */
: /* inputs */
[dataptr] "e" (values), 	// pointer to grb values
[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
[bytes]   "d" (size),		// number of bytes to send
[bits]    "d" (bitcount),   // number of bits/2
[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
    );

}

template< uint8_t array_size>
void send( rgb (&values)[array_size], uint8_t bit)
{
	send( &values[0], array_size, bit);
}

}



#endif /* WS2811_8_H_ */
