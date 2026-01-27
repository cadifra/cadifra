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
    ToLower<Char> toLower_;

public:
    CaseInsensitiveLess(const std::locale& l = std::locale()):
        toLower_(l) {}

    bool operator()(Char a, Char b) const
    {
        return toLower_(a) < toLower_(b);
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
    ToLower<Char> toLower_;

public:
    CaseInsensitiveEqual(const std::locale& l = std::locale()):
        toLower_(l) {}

    bool operator()(Char a, Char b) const
    {
        return toLower_(a) == toLower_(b);
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
