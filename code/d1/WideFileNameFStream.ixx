/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <stdio.h>

export module d1.WideFileNameFStream;

import std;


namespace d1
{

export template <class S> // std::basic_ostream, std::basic_istream or std::basic_iostream
class WideFileNameFStream: public S
{
    std::basic_filebuf<typename S::char_type, typename S::traits_type> fileBuf_;

public:
    WideFileNameFStream(const wchar_t* fileName, const wchar_t* mode):
        S(&fileBuf_),
        fileBuf_(_wfopen(fileName, mode)) // non-standard c-tor called!
    {
    }

    ~WideFileNameFStream()
    {
        fileBuf_.close();
    }

    // The WideFileNameFStream class has no member functions "open", "is_open"
    // and "close" like the classes std::basic_ofstream, std::basic_ifstream
    // and std::basic_fstream.
};

}
