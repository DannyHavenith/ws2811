//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * Library for bit-banging data to WS2811 led controllers.
 * This file contains a definition of the send() function for 8 Mhz controllers.
 */

#ifndef WS2811_8_H_
#define WS2811_8_H_
#include <avr/io.h>
#include "rgb.h"

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
    _delay_us( 40);

    // note: the labels in this piece of assembly code aren't very explanatory. The real documentation
    // of this code can be found in the spreadsheet ws2811@8Mhz.ods
    // The order of the blocks of code have been determined by arrange_timed_code.cpp
    asm volatile(
        "       LDI %[bits], 4"    "\n"
        "       LD __tmp_reg__, %a[dataptr]+"     "\n" // fill the first byte and determine first bit value
        "       LSL __tmp_reg__"   "\n"
        "       BRCC P0x19"  "\n"
        "       RJMP L1x00"   "\n"
        "L0005: SUBI %[bits], 1"         "\n"
        "       BRNE Mx008"      "\n"
        "       LD __tmp_reg__, %a[dataptr]+"    "\n"
        "       LDI %[bits], 4"  "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       SBIW %[bytes], 1"       "\n"
        "       BREQ Hx017"      "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCC P0x19"      "\n"
        "       RJMP L1x00"      "\n"
        "M1006: RJMP Mx008"     "\n"
//        "       NOP"     "\n"
        "Mx008: OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCS P1x17"      "\n"
        "       RJMP P0x18"      "\n"
        "Hx017: RJMP XX0"     "\n"
//        "       NOP"     "\n"
        "XX0:   NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       RJMP END"        "\n"
        "P0x18: OUT %[portout], %[downreg]"      "\n"
        "P0x19: NOP"     "\n"
        "L0x00: OUT %[portout], %[upreg]"        "\n"
        "       LSL __tmp_reg__"         "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       BRCC L0005"      "\n"
        "       SUBI %[bits], 1"         "\n"
        "       BRNE Mx107"      "\n"
        "       LD __tmp_reg__, %a[dataptr]+"    "\n"
        "       LDI %[bits], 4"  "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       SBIW %[bytes], 1"       "\n"
        "       BREQ Hx115"      "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCS P1x17"      "\n"
        "       RJMP P0x18"      "\n"
        "P1x17: NOP"     "\n"
        "P1x18: OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "L1x00: OUT %[portout], %[upreg]"        "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCS L1104"      "\n"
        "       SUBI %[bits], 1"         "\n"
        "       BRNE M1006"      "\n"
        "       LD __tmp_reg__, %a[dataptr]+"    "\n"
        "       LDI %[bits], 4"  "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       SBIW %[bytes], 1"       "\n"
        "       BREQ Hx017"      "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCC P0x19"      "\n"
        "       RJMP L1x00"      "\n"
        "L1104: SUBI %[bits], 1"         "\n"
        "       BRNE Mx107"      "\n"
        "       LD __tmp_reg__, %a[dataptr]+"    "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       LDI %[bits], 4"  "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       SBIW %[bytes], 1"       "\n"
        "       BREQ Hx115"      "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCS P1x17"      "\n"
        "       RJMP P0x18"      "\n"
        "Mx107: NOP"     "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       RJMP XX1"     "\n"
//        "       NOP"     "\n"
        "XX1:   NOP"     "\n"
        "       LSL __tmp_reg__"         "\n"
        "       BRCS P1x17"      "\n"
        "       RJMP P0x18"      "\n"
        "Hx115: RJMP XX2"     "\n"
//        "       NOP"     "\n"
        "XX2:   NOP"     "\n"
        "       OUT %[portout], %[downreg]"      "\n"
        "       NOP"     "\n"
        "       OUT %[portout], %[upreg]"        "\n"
        "       RJMP END"        "\n"
        "END:   NOP"	"\n"
: /* no output */
: /* inputs */
[dataptr] "e" (values), 	// pointer to grb values
[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
[bytes]   "w" (size),		// number of bytes to send
[bits]    "d" (bitcount),       // number of bits/2
[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
    );

}

}



#endif /* WS2811_8_H_ */
