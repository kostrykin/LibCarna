/*
 *  Copyright (C) 2010 - 2016 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 * 
 * 
 *  Copyright (C) 2021 - 2025 Leonid Kostrykin
 * 
 */

#pragma once

#include <LibCarna/LibCarna.hpp>
#include <ostream>
#include <istream>
#include <queue>
#include <cstdint>

namespace LibCarna
{

namespace testing
{

namespace HUIO
{



// ----------------------------------------------------------------------------------
// Types & Constants
// ----------------------------------------------------------------------------------

typedef uint16_t shifted_huv_t;

struct buffer_t
{
    unsigned int first : 12;
    unsigned int second : 12;
};

const std::streamsize BUFFER_LENGTH = 3;



// ----------------------------------------------------------------------------------
// Writer
// ----------------------------------------------------------------------------------

struct Writer
{

    Writer( std::ostream& out )
        : out( out )
    {
    }

    ~Writer()
    {
        flush();
    }

    void write( signed short huv )
    {
        const signed short shifted_huv = huv + 1024;
        buffered_shifted_huv.push( static_cast< shifted_huv_t >( shifted_huv ) );
        if( buffered_shifted_huv.size() % 2 == 0 )
        {
            flush();
        }
    }

private:

    std::ostream& out;

    std::queue< shifted_huv_t > buffered_shifted_huv;
    buffer_t buffer;

    void flush()
    {
        if( buffered_shifted_huv.empty() )
        {
            return;
        }

        buffer.first = buffered_shifted_huv.front();
        buffered_shifted_huv.pop();

        if( buffered_shifted_huv.empty() )
        {
            buffer.second = 0; // padding
        }
        else
        {
            buffer.second = buffered_shifted_huv.front();
            buffered_shifted_huv.pop();
        }

        out.write( reinterpret_cast< const char* >( &buffer ), BUFFER_LENGTH );
    }

};  // HUIO :: Writer



// ----------------------------------------------------------------------------------
// Reader
// ----------------------------------------------------------------------------------

struct Reader
{

    Reader( std::istream& in )
        : in( in )
    {
    }

    signed short read()
    {
        if( read_values.empty() )
        {
            readAhead();
        }
        const shifted_huv_t shifted_huv = read_values.front();
        read_values.pop();
        const signed short huv = static_cast< signed short >( shifted_huv ) - 1024;
        return huv;
    }

private:

    std::istream& in;

    buffer_t buffer;
    std::queue< shifted_huv_t > read_values;

    void readAhead()
    {
        in.read( reinterpret_cast< char* >( &buffer ), BUFFER_LENGTH );
        read_values.push( buffer.first );
        read_values.push( buffer.second );
    }

};  // HUIO :: Reader



}  // namespace HUIO

}  // namespace testing

}  // namespace LibCarna
