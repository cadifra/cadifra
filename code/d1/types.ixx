/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

// portable basic types and their names used by Cadifra

export module d1.types;

import std;

#ifndef _MSC_VER // Microsoft C++ Compiler
#error "your compiler is not supported"
#endif

export namespace d1
{

#ifdef _MSC_VER // Microsoft C++ Compiler

// Basic integer types.

using int8 = std::int8_t;
using uint8 = std::uint8_t;

using int16 = std::int16_t;
using uint16 = std::uint16_t;

using int32 = std::int32_t;
using uint32 = std::uint32_t;


// floating point types

using float64 = double; // IEEE 64 bit floating point

#endif


using bytestring = std::basic_string<uint8>;


}
