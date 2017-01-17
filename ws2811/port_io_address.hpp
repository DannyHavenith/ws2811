//
//  Copyright (C) 2017 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

/**
 * This file contains definitions of
 */
#ifndef WS2811_PORT_IO_ADDRESS_HPP_
#define WS2811_PORT_IO_ADDRESS_HPP_
#include <avr/io.h> // for _SFR_IO_ADDR
namespace ws2811
{

template<uint16_t address>
struct PortIoAddress
{
    static const uint16_t value = address;
};

#define WS2811_DECLARE_PORT( port_)                                       \
    typedef PortIoAddress< _SFR_IO_ADDR( PORT ## port_)> AtPort ## port_ ;\
    AtPort ## port_ __attribute__((unused)) port ## port_;                                        \
    /**/


namespace
{
#if defined( PORTA)
    WS2811_DECLARE_PORT( A)
#endif
#if defined( PORTB)
    WS2811_DECLARE_PORT( B)
#endif
#if defined( PORTC)
    WS2811_DECLARE_PORT( C)
#endif
#if defined( PORTD)
    WS2811_DECLARE_PORT( D)
#endif
#if defined( PORTE)
    WS2811_DECLARE_PORT( E)
#endif
#if defined( PORTF)
    WS2811_DECLARE_PORT( F)
#endif
}

}



#endif /* WS2811_PORT_IO_ADDRESS_HPP_ */
