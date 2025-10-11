/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Color;

import d1.types;

import std;


namespace d1
{

export struct Color
{
    d1::uint8 red = 255, green = 255, blue = 255;

    bool operator==(const Color&) const = default;

    std::wstring str() const;

    friend std::wostream& operator<<(std::wostream& s, const Color& c)
    {
        s << c.str();
        return s;
    }
};

}
