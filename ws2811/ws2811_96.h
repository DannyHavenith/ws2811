//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * Library for bit-banging data to WS2811 led controllers.
 * This file contains two implementations of the send() function for ws2811 controllers.
 *
 * The first implementation, send() expects an array of GRB-values and will send those
 * to the given output pin.
 *
 * The second implementation send_sparse() expects an array filled with blocks of LEDS,
 * interspersed with zero LED values.
 */

#ifndef WS2811_96_H_
#define WS2811_96_H_
#include <avr/io.h>
#include <util/delay_basic.h>

#include "../ws2811/rgb.h"

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
    uint8_t bitcount = 7;
    WS2811_PORT = low_val;
    _delay_loop_1( 384/3); // 40us = 384 ticks, 3 ticks per loop

    // The documentationof this code, including a graphical representation of the waveform
    // can be found in the spreadsheet ws2811@9.6Mhz.ods, in the tab "reduced footprint  9.6"
    // Note that if a label ends with a double digit (e.g. "cont09"), those last digits represent
    // the phase of the waveform that the code at that label position is in.
    // Jumps to labels of the form spbrknn are there to have single instruction words that take 2 cycles
    asm volatile(
    		"spstart:  LD __tmp_reg__, %a[dataptr]+            \n"
    		"sps00:    OUT %[portout], %[upreg]                \n" //    at this point the bits are in '__tmp_reg__'
    		"          LSL __tmp_reg__                         \n" //    get leftmost of the remaining bits
    		"          BRCS spskip04                           \n" //    skip the next instruction if it is 1
    		"          OUT %[portout], %[downreg]              \n" //    pull the line down if it was a zero
    		"spskip04: RJMP spbrk0                             \n"
    		"spbrk0:   SUBI %[bits], 1                         \n" //    decrease bit counter...
    		"          BRNE spcont09                           \n" //    ...and make sure we loop if it's not zero yet
    		"          LDI %[bits], 7                          \n" //    bitcounter was zero, reset to 7
    		"          OUT %[portout], %[downreg]              \n" //    has no effect if the line was already down
    		"          RJMP spbrk10                            \n"
    		"spcont09: OUT %[portout], %[downreg]              \n"
    		"          RJMP sps00                              \n"
    		"spbrk10:  OUT %[portout], %[upreg]                \n"
    		"          LSL __tmp_reg__                         \n" //    get the final bit
    		"          BRCS spskip14                           \n"
    		"          OUT %[portout], %[downreg]              \n"
    		"spskip14: NOP                                     \n"
    		"          LD __tmp_reg__, %a[dataptr]+            \n" //    load either next data byte or zero count
    		"          SBIW %[bytes], 1                        \n" //    do we need to send another byte?
    		"          OUT %[portout], %[downreg]              \n"
    		"          BRNE sps00                              \n" //    jump to the start if we do.
    		"          NOP                                     \n"
    		"spend:    OUT %[portout], %[downreg]              \n"

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

////////////////////////////////////////////////////////////////////////////////
// This part of the file contains functions for sparse LED string buffers.

/**
 * A data structure for a sparse representation of values in a LED string.
 *
 * A non-sparse representation of an LED string holds 3 bytes for every
 * LED in the string. For a 60 LED string this means that at least 180 bytes
 * of memory are required.
 *
 * This type also has a buffer of bytes, but these bytes hold sequences of
 * the form:
 * <jump>, <count>, G1, R1, B1, G2, R2, B2, <jump>, <count>, G3, R3, B3, etc...
 *
 * <jump> represents an amount of black pixels, <count> is the number of GRB values
 * that follow
 *
 * A buffer is terminated by a zero <jump> value or a zero <count>. The first <jump>
 * never terminates the sequence, even if it is zero. A zero <jump> count at the start
 * means the string starts with a lit LED.
 */
template<uint8_t buffer_size, uint8_t led_string_size>
struct sparse_leds
{
	uint8_t buffer[buffer_size];

	/// move the range [begin, end> towards buffer_end.
	/// buffer_end must be higher than end, for this function to work correctly.
	/// The bytes that become available are filled with zeros.
	static void move_right( uint8_t *begin, uint8_t *end, uint8_t *buffer_end)
	{
		while (begin < end) *(--buffer_end) = *(--end);
		while( begin < buffer_end) *(--buffer_end) = 0;
	}
};

/**
 * specialization of the led_buffer_traits for sparse buffers.
 *
 * For regular arrays, the led string size is simply the number of bytes
 * divided by three. A sparse buffer has the number of leds encoded in the
 * type as a template argument.
 */
template< uint8_t buffer_size, uint8_t led_string_size>
struct led_buffer_traits<sparse_leds<buffer_size, led_string_size> >
{
	static const uint8_t count = led_string_size;
	static const uint8_t size = buffer_size;
};

/**
 * clear a sparse buffer. This fills the buffer with as many
 * black leds as there are leds in the string.
 */
template<uint8_t buffer_size, uint8_t led_string_size>
inline void clear( sparse_leds<buffer_size, led_string_size> &leds)
{
	leds.buffer[0] = led_string_size;
	leds.buffer[1] = 0;
}


/**
 * Given a sparse buffer and a position in the LED string, find the position in
 * the buffer that corresponds with this LED. If such a position did not exist, it
 * will be created by introducing a new block inside the buffer or by appending
 * a location for the LED at the start or end of an existing block.
 *
 * This function assumes that the current buffer already covers the complete LED
 * string, or in other words, the sum of all <jump> and <count> values must be higher
 * than the argument 'position' to this function. This function makes sure that
 * the sum of <jump>s and <count>s remains the same.
 *
 * This function is deliberately not implemented as an operator[] of sparse_leds, because
 * using an explicit function call makes it clear that code is being run and that it is
 * worthwhile to store the result of this function instead of calling the function twice.
 * Measurements have shown that the compiler will not memoize a second call to this function with
 * the same arguments.
 */
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
			leds.move_right( buffer_iterator, end - 5, end);
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
			leds.move_right( buffer_iterator, end-3, end);
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
				leds.move_right( buffer_iterator, end - 1, end);
			}
			else
			{
				// distance is non-zero, just enlarge the current block
				++*buffer_iterator;
				buffer_iterator += jump_pos;
				leds.move_right( buffer_iterator, end - 3, end);
			}
			break;
		}
		buffer_iterator+=jump_pos;
	}

	return *(reinterpret_cast<rgb *>(buffer_iterator));
}

/**
 * Send a sparse buffer, containing blocks of LED values interspersed with counts of
 * black LEDs to a WS2811 string, using bit 'bit' of the port determined by the macro
 * WS2811_PORT.
 */
void send_sparse( const void *buffer, uint8_t bit )
{
    const uint8_t mask =_BV(bit);
    uint8_t low_val = WS2811_PORT & (~mask);
    uint8_t high_val = WS2811_PORT | mask;


    WS2811_PORT = low_val;
    _delay_loop_1(384/3); // 40us = 384 ticks, 3 ticks per loop

    // The documentation of this code, including a graphical representation of the waveform
    // can be found in the spreadsheet ws2811@9.6Mhz.ods, in the tab "compact 9.6"
    // Note that if a label ends with a double digit (e.g. "cont09"), those last digits represent
    // the phase of the waveform that the code at that label position is in.
    // Jumps to labels of the form brk<n> are there to have single instruction words that take 2 cycles
    asm volatile(
    		"start:  LDI %[bits], 23                         \n"
    		"        LD %[data], %a[dataptr]+                \n" //    read how many zero-leds we need to transmit
    		"        TST %[data]                             \n" //
    		"        BRNE z00                                \n" //    Jump to the zero routine
    		"        LDI %[bits], 7                          \n" //
    		"        RJMP z11                                \n"
    		"cont09: OUT %[portout], %[downreg]              \n"
    		"        RJMP s00                                \n"
    		"zcont09:NOP                                     \n"
    		"        RJMP z00                                \n"
    		"s00:    OUT %[portout], %[upreg]                \n" //    at this point the bits are in '[data]'
    		"        LSL %[data]                             \n" //    get leftmost of the remaining bits
    		"        BRCS skip04                             \n" //    skip the next instruction if it is 1
    		"        OUT %[portout], %[downreg]              \n" //    pull the line down if it was a zero
    		"skip04: RJMP brk0                               \n" //
    		"brk0:   SUBI %[bits], 1                         \n" //    decrease bit counter...
    		"        BRNE cont09                             \n" //    ...and make sure we loop if it's not zero yet
    		"        LDI %[bits], 7                          \n" //    bitcounter was zero, reset to 7
    		"        OUT %[portout], %[downreg]              \n" //    has no effect if the line was already down
    		"        RJMP brk1                               \n" //
    		"brk1:   OUT %[portout], %[upreg]                \n" //
    		"        LSL %[data]                             \n" //    get the final bit
    		"        BRCS skip14                             \n"
    		"        OUT %[portout], %[downreg]              \n" //
    		"skip14: RJMP brk2                               \n" //
    		"brk2:   LD %[data], %a[dataptr]+                \n" //    load either next [data] byte or zero count
    		"        SUBI %[bytes], 1                        \n" //    do we need to send another byte?
    		"        OUT %[portout], %[downreg]              \n" //
    		"        BRNE s00                                \n" //    jump to the start if we do.
    		"        LDI %[bits], 23                         \n" //    prepare for sending 24 bits of zero data
    		"z00:    OUT %[portout], %[upreg]                \n" //
    		"        TST %[data]                             \n" //    check how many zeros we have to send
    		"        BREQ end                                \n" //    jump out if it there are none to send.
    		"        OUT %[portout], %[downreg]              \n" //
    		"        RJMP brk3                               \n" //
    		"brk3:   SUBI %[bits], 1                         \n" //
    		"        BRNE zcont09                            \n" //
    		"        SUBI %[data], 1                         \n" //    [data] actually contains 'byte' count (in 3 bytes units)
    		"        LDI %[bits], 24                         \n" //    24, because we are not falling into the lower half
    		"        BRNE z00                                \n" //
    		"        LDI %[bits], 7                          \n" //
    		"        OUT %[portout], %[upreg]                \n" //
    		"z11:    LD %[data], %a[dataptr]+                \n" //    read the number of leds
    		"        OUT %[portout], %[downreg]              \n" //
    		"        MOV %[bytes], %[data]                   \n" //    multiply by three
    		"        ADD %[bytes], %[data]                   \n" //
    		"        ADD %[bytes], %[data]                   \n" //
    		"        LD %[data], %a[dataptr]+                \n" //    read the first byte
    		"        BREQ z1b                                \n" //    but jump out if the byte count was zero...
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

/**
 * Interface adapter that allows sending a sparse buffer by using the send() function.
 */
template<uint8_t buffer_size, uint8_t led_string_size>
inline void send( const sparse_leds<buffer_size, led_string_size> &leds, uint8_t channel)
{
	send_sparse( leds.buffer,  channel);
}

}

#endif /* WS2811_96_H_ */
