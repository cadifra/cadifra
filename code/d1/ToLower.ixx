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

    const std::locale loc_;
    const Facet& facet_;

public:
    ToLower(const std::locale& l = std::locale{}):
        loc_{ l },
        facet_{ std::use_facet<Facet>(loc_) }
    {
    }

    ToLower(const ToLower& rhs):
        loc_{ rhs.loc_ },
        facet_{ std::use_facet<Facet>(loc_) }
    {
    }

    Char operator()(Char c) const
    {
        return facet_.tolower(c);
    }

    ToLower& operator=(const ToLower&) = delete;
};



export template <typename String = std::wstring>
class ToLowerString
{
    ToLower<typename String::value_type> toLower_;

public:
    ToLowerString(const std::locale& l = std::locale{}):
        toLower_{ l }
    {
    }

    String operator()(const String& s) const
    {
        auto res = s;
        std::transform(begin(res), end(res), begin(res), toLower_);
        return res;
    }

    void convert(String& s) const
    {
        std::transform(begin(s), end(s), begin(s), toLower_);
    }
};


}
