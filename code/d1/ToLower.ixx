/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.ToLower;

import std;


namespace d1
{

export template <typename Char = wchar_t>
class ToLower
{
    using Facet = std::ctype<Char>;

    const std::locale itsLoc;
    const Facet& itsFacet;

public:
    ToLower(const std::locale& l = std::locale{}):
        itsLoc{ l },
        itsFacet{ std::use_facet<Facet>(itsLoc) }
    {
    }

    ToLower(const ToLower& rhs):
        itsLoc{ rhs.itsLoc },
        itsFacet{ std::use_facet<Facet>(itsLoc) }
    {
    }

    Char operator()(Char c) const
    {
        return itsFacet.tolower(c);
    }

    ToLower& operator=(const ToLower&) = delete;
};



export template <typename String = std::wstring>
class ToLowerString
{
    ToLower<typename String::value_type> itsToLower;

public:
    ToLowerString(const std::locale& l = std::locale{}):
        itsToLower{ l }
    {
    }

    String operator()(const String& s) const
    {
        auto res = s;
        std::transform(begin(res), end(res), begin(res), itsToLower);
        return res;
    }

    void Convert(String& s) const
    {
        std::transform(begin(s), end(s), begin(s), itsToLower);
    }
};


}
