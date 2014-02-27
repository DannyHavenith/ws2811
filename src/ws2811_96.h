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
#if !defined(WS2811_SPARSE)
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

#else // defined WS2811_SPARSE
void send_sparse( void *data, uint8_t bit )
{
    const uint8_t mask =_BV(bit);
    uint8_t low_val = WS2811_PORT & (~mask);
    uint8_t high_val = WS2811_PORT | mask;


    // reset the controllers by pulling the data line low
    uint8_t bitcount = 4;
    WS2811_PORT = low_val;
    _delay_loop_1( 128); // 40us = 384 ticks, 3 ticks per loop

    // note: the labels in this piece of assembly code aren't very explanatory. The real documentation
    // of this code can be found in the spreadsheet ws2811@9.6Mhz.ods, in the tab "sparse"
    // The order of the blocks of code have been determined by arrange_timed_code.cpp
    asm volatile(
    		"        RJMP start                              \n"
    		"brk12:  RJMP brk13                              \n"
    		"Mx009:  OUT %[portout], %[downreg]              \n"
    		"        RJMP brk2                               \n"
    		"brk3:   OUT %[portout], %[downreg]              \n"
    		"        NOP                                     \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCS P1x18                              \n"
    		"        RJMP P0x19                              \n"
    		"brk0:   OUT %[portout], %[upreg]                \n"
    		"        RJMP brk1                               \n"
    		"brk5:   RJMP brk6                               \n"
    		"Hx116:  LD %[bytes], %a[dataptr]+               \n"
    		"        OR %[bytes], %[bytes]                   \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        LDI %[bits], 12                         \n"
    		"        NOP                                     \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"        BRNE LZ03                               \n"
    		"        RJMP END                                \n"
    		"MZ07:   RJMP brk9                               \n"
    		"brk8:   OUT %[portout], %[upreg]                \n"
    		"        NOP                                     \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        NOP                                     \n"
    		"        BREQ Hx116                              \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCS P1x18                              \n"
    		"        RJMP P0x19                              \n"
    		"L0006:  SUBI %[bits], 1                         \n"
    		"        BRNE Mx009                              \n"
    		"        LD __tmp_reg__, %a[dataptr]+            \n"
    		"        NOP                                     \n"
    		"        LDI %[bits], 4                          \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"        RJMP brk7                               \n"
    		"L0x00:  OUT %[portout], %[upreg]                \n"
    		"        NOP                                     \n"
    		"        LSL __tmp_reg__                         \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        BRCC L0006                              \n"
    		"        SUBI %[bits], 1                         \n"
    		"        BRNE Mx108                              \n"
    		"        LD __tmp_reg__, %a[dataptr]+            \n"
    		"        LDI %[bits], 4                          \n"
    		"        RJMP brk8                               \n"
    		"L1105:  SUBI %[bits], 1                         \n"
    		"        BRNE Mx108                              \n"
    		"        LD __tmp_reg__, %a[dataptr]+            \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        NOP                                     \n"
    		"        LDI %[bits], 4                          \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"        NOP                                     \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        NOP                                     \n"
    		"        BREQ Hx116                              \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCS P1x18                              \n"
    		"        RJMP P0x19                              \n"
    		"P1x18:  NOP                                     \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        NOP                                     \n"
    		"P1x1b:  NOP                                     \n"
    		"L1x00:  OUT %[portout], %[upreg]                \n"
    		"        NOP                                     \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCS L1105                              \n"
    		"        SUBI %[bits], 1                         \n"
    		"        BRNE M1007                              \n"
    		"        LD __tmp_reg__, %a[dataptr]+            \n"
    		"        LDI %[bits], 4                          \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        RJMP brk0                               \n"
    		"LZ03:   OUT %[portout], %[downreg]              \n"
    		"        SUBI %[bits], 1                         \n"
    		"        BRNE MZ07                               \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        LDI %[bits], 12                         \n"
    		"        NOP                                     \n"
    		"        BRNE MZ0b                               \n"
    		"        LD %[bytes], %a[dataptr]+               \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"        OR %[bytes], %[bytes]                   \n"
    		"        BREQ TOEND                              \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        NOP                                     \n"
    		"LZ15:   LDI %[bits], 4                          \n"
    		"        LD __tmp_reg__, %a[dataptr]+            \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCS P1x1b                              \n"
    		"        RJMP L0x00                              \n"
    		"brk11:  RJMP brk12                              \n"
    		"brk13:  RJMP LZ00                               \n"
    		"brk4:   OUT %[portout], %[upreg]                \n"
    		"        RJMP brk5                               \n"
    		"brk9:   RJMP MZ0b                               \n"
    		"Mx108:  NOP                                     \n"
    		"        OUT %[portout], %[downreg]              \n"
    		"        RJMP brk4                               \n"
    		"start:  LD %[bytes], %a[dataptr]+               \n"
    		"        OR %[bytes], %[bytes]                   \n"
    		"        BRNE LZ00                               \n"
    		"        RJMP LZ15                               \n"
    		"LZ00:   OUT %[portout], %[upreg]                \n"
    		"        RJMP LZ03                               \n"
    		"M1007:  RJMP Mx009                              \n"
    		"brk10:  OUT %[portout], %[downreg]              \n"
    		"        RJMP brk11                              \n"
    		"brk6:   LSL __tmp_reg__                         \n"
    		"        BRCS P1x18                              \n"
    		"        RJMP P0x19                              \n"
    		"brk1:   OUT %[portout], %[downreg]              \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        NOP                                     \n"
    		"        BREQ Hx018                              \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCC P0x1a                              \n"
    		"        RJMP P1x1b                              \n"
    		"Hx018:  LD %[bytes], %a[dataptr]+               \n"
    		"        OR %[bytes], %[bytes]                   \n"
    		"        LDI %[bits], 12                         \n"
    		"        OUT %[portout], %[upreg]                \n"
    		"        BRNE LZ03                               \n"
    		"TOEND:  RJMP END                                \n"
    		"MZ0b:   NOP                                     \n"
    		"MZ10:   OUT %[portout], %[upreg]                \n"
    		"        RJMP brk10                              \n"
    		"brk7:   OUT %[portout], %[downreg]              \n"
    		"        SUBI %[bytes], 1                        \n"
    		"        NOP                                     \n"
    		"        BREQ Hx018                              \n"
    		"        LSL __tmp_reg__                         \n"
    		"        BRCC P0x1a                              \n"
    		"        RJMP P1x1b                              \n"
    		"P0x19:  OUT %[portout], %[downreg]              \n"
    		"P0x1a:  RJMP L0x00                              \n"
    		"brk2:   OUT %[portout], %[upreg]                \n"
    		"        RJMP brk3                               \n"
    		"END:                                            \n"
    		: /* no output */
    		: /* inputs */
    		[dataptr] "e" (data), 	// pointer to grb values
    		[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
    		[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
    		[bytes]   "d" (0),			// number of bytes to send
    		[bits]    "d" (bitcount),       // number of bits/2
    		[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
     );
}
#endif // defined WS2811_SPARSE
}

#endif /* WS2811_96_H_ */
