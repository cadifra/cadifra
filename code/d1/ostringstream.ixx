/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.ostringstream;

import std;


// Defines class ostringstream, an ostream that writes to a string.

// Alternative to std::ostringstream.


namespace d1
{


class ostringstreambuf: public std::streambuf
{
    std::string& str_;

public:
    ostringstreambuf(std::string& s):
        str_(s)
    {
    }

    //-- std::streambuf

    int_type overflow(int_type meta) override
    {
        if (not traits_type::eq_int_type(meta, traits_type::eof()))
            str_.push_back(traits_type::to_char_type(meta));
        return meta;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override
    {
        str_.append(s, static_cast<size_t>(n));
        return n;
    }
};


class ostringstreambufV
{
protected:
    ostringstreambuf buf_;
    ostringstreambufV(std::string& s):
        buf_(s)
    {
    }
};


export class ostringstream: private ostringstreambufV, public std::ostream
{
    using base2 = std::ostream;

public:
    ostringstream(std::string& s):
        ostringstreambufV(s),
        base2(&buf_)
    {
    }
};


}
