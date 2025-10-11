/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CaseInsensitive;

import d1.ToLower;

import std;


namespace d1
{

export template <typename String = std::wstring>
class CaseInsensitiveLess
{
    using Char = String::value_type;
    ToLower<Char> itsToLower;

public:
    CaseInsensitiveLess(const std::locale& l = std::locale()):
        itsToLower(l) {}

    bool operator()(Char a, Char b) const
    {
        return itsToLower(a) < itsToLower(b);
    }

    bool operator()(const String& a, const String& b) const
    {
        return std::lexicographical_compare(
            begin(a), end(a), begin(b), end(b), *this);
    }
};



export template <typename String = std::wstring>
class CaseInsensitiveEqual
{
    using Char = String::value_type;
    ToLower<Char> itsToLower;

public:
    CaseInsensitiveEqual(const std::locale& l = std::locale()):
        itsToLower(l) {}

    bool operator()(Char a, Char b) const
    {
        return itsToLower(a) == itsToLower(b);
    }

    bool operator()(const String& a, const String& b) const
    {
        if (a.size() != b.size())
            return false;
        return std::equal(
            begin(a), end(a), begin(b), *this);
    }
};


}
