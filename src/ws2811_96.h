//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * Library for bit-banging data to WS2811 led controllers.
 * This file contains a definition of the send() function for 9.6 Mhz controllers.
 */

#ifndef WS2811_96_H_
#define WS2811_96_H_
#include <avr/io.h>
#include <util/delay_basic.h>

#include "rgb.h"

#define WS2811_SPARSE

namespace ws2811
{

/**
 * This function sends the RGB-data in an array of rgb structs through
 * the given io-pin.
 * The port is determined by the macro WS2811_PORT, but the actual pin to
 * be used is an argument to this function. This allows a single instance of this function
 * to control up to 8 separate channels.
 */
void send( const void *values, uint16_t array_size, uint8_t bit)
{
    const uint8_t mask =_BV(bit);
    uint8_t low_val = WS2811_PORT & (~mask);
    uint8_t high_val = WS2811_PORT | mask;
    uint16_t size = array_size * sizeof( rgb); // size in bytes


    // reset the controllers by pulling the data line low
    uint8_t bitcount = 4;
    WS2811_PORT = low_val;
    _delay_loop_1( 128); // 40us = 384 ticks, 3 ticks per loop

    // note: the labels in this piece of assembly code aren't very explanatory. The real documentation
    // of this code can be found in the spreadsheet ws2811@8Mhz.ods
    // The order of the blocks of code have been determined by arrange_timed_code.cpp
    // jumps to labels of the form XXn are there to have single instruction words that take 2 cycles
    asm volatile(
	"		LDI %[bits], 4"		"\n"
	"		LD __tmp_reg__, %a[dataptr]+"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCS P1x1b"		"\n"
	"		RJMP L0x00"		"\n"
	"L0006:	SUBI %[bits], 1"		"\n"
	"		BRNE Mx009"		"\n"
	"		LD __tmp_reg__, %a[dataptr]+"		"\n"
	"		NOP"		"\n"
	"		LDI %[bits], 4"		"\n"
	"		OUT %[portout], %[upreg]"		"\n"
	"		RJMP XX1"		"\n"
	"XX1:	OUT %[portout], %[downreg]"		"\n"
	"		SBIW %[bytes], 1"		"\n"
	"		BREQ Hx018"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCC P0x1a"		"\n"
	"		RJMP P1x1b"		"\n"
	"M1007:	RJMP Mx009"		"\n"
	"Mx009:	OUT %[portout], %[downreg]"		"\n"
	"		RJMP XX2"		"\n"
	"XX2:	OUT %[portout], %[upreg]"		"\n"
	"		RJMP XX3"		"\n"
	"XX3:	OUT %[portout], %[downreg]"		"\n"
	"		NOP"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCS P1x18"		"\n"
	"		RJMP P0x19"		"\n"
	"Hx018:	RJMP XX4"		"\n"
	"XX4:	RJMP XX5"		"\n"
	"XX5:	OUT %[portout], %[upreg]"		"\n"
	"		RJMP END"		"\n"
	"P0x19:	OUT %[portout], %[downreg]"		"\n"
	"P0x1a:	RJMP L0x00"		"\n"
	"L0x00:	OUT %[portout], %[upreg]"		"\n"
	"		NOP"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		BRCC L0006"		"\n"
	"		SUBI %[bits], 1"		"\n"
	"		BRNE Mx108"		"\n"
	"		LD __tmp_reg__, %a[dataptr]+"		"\n"
	"		LDI %[bits], 4"		"\n"
	"		RJMP XX6"		"\n"
	"XX6:	OUT %[portout], %[upreg]"		"\n"
	"		NOP"		"\n"
	"		SBIW %[bytes], 1"		"\n"
	"		BREQ Hx116"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCS P1x18"		"\n"
	"		RJMP P0x19"		"\n"
	"P1x18:	NOP"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		NOP"		"\n"
	"P1x1b:	NOP"		"\n"
	"L1x00:	OUT %[portout], %[upreg]"		"\n"
	"		NOP"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCS L1105"		"\n"
	"		SUBI %[bits], 1"		"\n"
	"		BRNE M1007"		"\n"
	"		LD __tmp_reg__, %a[dataptr]+"		"\n"
	"		LDI %[bits], 4"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		RJMP XX7"		"\n"
	"XX7:	OUT %[portout], %[upreg]"		"\n"
	"		RJMP XX8"		"\n"
	"XX8:	OUT %[portout], %[downreg]"		"\n"
	"		SBIW %[bytes], 1"		"\n"
	"		BREQ Hx018"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCC P0x1a"		"\n"
	"		RJMP P1x1b"		"\n"
	"L1105:	SUBI %[bits], 1"		"\n"
	"		BRNE Mx108"		"\n"
	"		LD __tmp_reg__, %a[dataptr]+"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		NOP"		"\n"
	"		LDI %[bits], 4"		"\n"
	"		OUT %[portout], %[upreg]"		"\n"
	"		NOP"		"\n"
	"		SBIW %[bytes], 1"		"\n"
	"		BREQ Hx116"		"\n"
	"		LSL __tmp_reg__"		"\n"
	"		BRCS P1x18"		"\n"
	"		RJMP P0x19"		"\n"
	"Mx108:	NOP"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		RJMP XX13"		"\n"
	"XX13:	OUT %[portout], %[upreg]"		"\n"
	"		RJMP XX9"		"\n"
	"XX9:	RJMP XX10"		"\n"
	"XX10:	LSL __tmp_reg__"		"\n"
	"		BRCS P1x18"		"\n"
	"		RJMP P0x19"		"\n"
	"Hx116:	RJMP XX11"		"\n"
	"XX11:	NOP"		"\n"
	"		OUT %[portout], %[downreg]"		"\n"
	"		RJMP XX12"		"\n"
	"XX12:	OUT %[portout], %[upreg]"		"\n"
	"		RJMP END"		"\n"
	"END:	"		"\n"

: /* no output */
: /* inputs */
[dataptr] "e" (values), 	// pointer to grb values
[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
[bytes]   "w" (size),			// number of bytes to send
[bits]    "d" (bitcount),       // number of bits/2
[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
    );

}


/**
 * A data structure for a sparse representation of values in a LED string.
 *
 * A non-sparse representation of an LED string holds 3 bytes for every
 * LED in the string. For a 60 LED string this means that at least 180 bytes
 * of memory are required.
 */
template<uint8_t buffer_size, uint8_t led_string_size>
struct sparse_leds
{
	uint8_t buffer[buffer_size];
};


template< uint8_t buffer_size, uint8_t led_string_size>
struct led_buffer_traits<sparse_leds<buffer_size, led_string_size> >
{
	static const uint8_t count = led_string_size;
	static const uint8_t size = buffer_size;
};

template<uint8_t buffer_size, uint8_t led_string_size>
inline void clear( sparse_leds<buffer_size, led_string_size> &leds)
{
	leds.buffer[0] = led_string_size;
	leds.buffer[1] = 0;
}


void move_right( uint8_t *begin, uint8_t *end, uint8_t *buffer_end)
{
	while (begin < end) *(--buffer_end) = *(--end);
	while( begin < buffer_end) *(--buffer_end) = 0;
}

template<uint8_t buffer_size, uint8_t led_string_size>
rgb & get( sparse_leds<buffer_size, led_string_size> &leds, uint8_t position)
{

	uint8_t *buffer_iterator = &leds.buffer[0];
	uint8_t * const end = buffer_iterator + buffer_size;
	uint8_t current_pos = 0;
	while (buffer_iterator < end)
	{
		// current_pos pointing one past the previous block
		// buffer_iterator pointing at a 'jump'

		uint8_t jump_pos = current_pos + *buffer_iterator;
		if (jump_pos  > position + 1)
		{
			// need to add a new block before the one we're pointing at.
			*buffer_iterator = (jump_pos - position - 1);
			move_right( buffer_iterator, end - 5, end);
			*buffer_iterator++ = position - current_pos;
			*buffer_iterator++=1;
			break;
		}
		current_pos = jump_pos;
		// current_pos now pointing at the location of the first led in the block
		if (current_pos  == position + 1)
		{
			// prepend the new led to the current block of leds
			--(*buffer_iterator);
			++buffer_iterator;
			++*buffer_iterator;
			++buffer_iterator;
			move_right( buffer_iterator, end-3, end);
			break;
		}
		++buffer_iterator; // pointing at the led block size
		jump_pos = position - current_pos;
		if ( jump_pos < *buffer_iterator)
		{
			// we're actually pointing to a led that is inside the current block
			buffer_iterator += (1 + 3 * jump_pos);
			break;
		}
		current_pos += *buffer_iterator; // pointing one past the last led in the string.
		jump_pos = 3 * (*buffer_iterator) + 1;
		//buffer_iterator += 3 * (*buffer_iterator);
		if (position == current_pos)
		{
			// decrease distance between this block and the next by one
			if (--(*(buffer_iterator+jump_pos)) == 0)
			{
				// distance is zero, concatenate two blocks.

				// new size is the old size plus the size of the next block plus
				// 1 for the new led.
				*buffer_iterator += *(buffer_iterator + jump_pos + 1)+1;
				buffer_iterator += jump_pos;
				move_right( buffer_iterator, end - 1, end);
			}
			else
			{
				// distance is non-zero, just enlarge the current block
				++*buffer_iterator;
				buffer_iterator += jump_pos;
				move_right( buffer_iterator, end - 3, end);
			}
			break;
		}
		buffer_iterator+=jump_pos;
	}

	return *(reinterpret_cast<rgb *>(buffer_iterator));

}

void send_sparse( const void *buffer, uint8_t bit )
{
    const uint8_t mask =_BV(bit);
    uint8_t low_val = WS2811_PORT & (~mask);
    uint8_t high_val = WS2811_PORT | mask;


    WS2811_PORT = low_val;
    _delay_loop_1(384/3); // 40us = 384 ticks, 3 ticks per loop

    // note: the labels in this piece of assembly code aren't very explanatory. The real documentation
    // of this code can be found in the spreadsheet ws2811@9.6Mhz.ods, in the tab "sparse"
    // The ordrer of the blocks of code have been determined by arrange_timed_code.cpp
    asm volatile(
    		"start:  LDI %[bits], 23                         \n"
    		"        LD %[data], %a[dataptr]+                \n"
    		"        TST %[data]                     \n"
    		"        BRNE z00                                \n"
    		"        LDI %[bits], 7                          \n"
    		"        RJMP z11                                \n"
    		"cont09: OUT %[portout], %[downreg]              \n"
    		"        RJMP s00                                \n"
    		"zcont09:NOP                                     \n"
    		"        RJMP z00                                \n"
    		"s00:    OUT %[portout], %[upreg]                \n"
    		"        LSL %[data]                             \n"
    		"        BRCS skip04                             \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"skip04: RJMP brk0                               \n"
    		"brk0:   SUBI %[bits], 1                         \n"
    		"        BRNE cont09                             \n"
    		"        LDI %[bits], 7                          \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        RJMP brk1                               \n"
    		"brk1:   OUT %[portout], %[upreg]                \n"
    		"        LSL %[data]                         \n"
    		"        BRCS skip14                             \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"skip14: RJMP brk2                               \n"
    		"brk2:   LD %[data], %a[dataptr]+            \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        BRNE s00                                \n"
    		"        LDI %[bits], 23                         \n"
    		"z00:    OUT %[portout], %[upreg]                \n"
    		"        TST %[data]             \n"
    		"        BREQ end                                \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        RJMP brk3                               \n"
    		"brk3:   SUBI %[bits], 1                         \n"
    		"        BRNE zcont09                            \n"
    		"        SUBI %[data], 1                     \n"
    		"        LDI %[bits], 24                         \n"
    		"        BRNE z00                                \n"
    		"        LDI %[bits], 7                          \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"z11:    LD %[data], %a[dataptr]+            \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        MOV %[bytes], %[data]               \n"
    		"        ADD %[bytes], %[data]               \n"
    		"        ADD %[bytes], %[data]               \n"
    		"        LD %[data], %a[dataptr]+            \n"
    		"        BREQ z1b                                \n"
    		"        RJMP s00                                \n"
    		"z1b:    NOP                                     \n"
    		"end:    OUT %[portout], %[upreg]                \n"
    		: /* no output */
    		: /* inputs */
    		[dataptr] "e" (buffer), 	// pointer to grb values
    		[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
    		[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
    		[bytes]   "d" (0),			// number of bytes to send
    		[bits]    "d" (1),       // number of bits
    		[data]	  "d" (2),
    		[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
     );


}
template<uint8_t buffer_size, uint8_t led_string_size>
inline void send( const sparse_leds<buffer_size, led_string_size> &leds, uint8_t channel)
{
	send_sparse( leds.buffer,  channel);
}

}

#endif /* WS2811_96_H_ */
