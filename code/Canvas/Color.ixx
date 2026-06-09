/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <gdiplus.h>

#include "d1/d1assert.h"

export module Canvas.Color;

import d1.types;


namespace Canvas
{

export class Color
{
public:
    enum AutomaticColor;
    Color(AutomaticColor ac = WINDOWTEXT);

    Color(d1::uint8 red, d1::uint8 green, d1::uint8 blue);
    // 0 = minimum intensity, 255 = maximum intensity

    explicit Color(d1::uint32 color_bitarray);
    // bit mask for blue : 0x00FF0000
    // bit mask for green: 0x0000FF00
    // bit mask for red  : 0x000000FF
    //
    // 0 = minimum intensity, 255 = maximum intensity
    //
    // Precondition: color_bitarray & 0xFF000000 == 0

    Color(const Color& c);
    Color& operator=(const Color& c);

    bool isAutomatic() const;

    d1::uint32 getRGB() const;
    // Precondition: isAutomatic() == false;

    AutomaticColor getAutomaticColor() const;
    // Precondition: isAutomatic() == true;


    bool operator==(const Color& c) const;
    bool operator<(const Color& c) const; // allows sorting


    // predefined colors:
    const static Color Black, White, Red, Green, Blue, Yellow;

    enum AutomaticColor
    {
        // see the OS function GetSysColor for a description of the values
        SCROLLBAR = 0,
        BACKGROUND = 1,
        ACTIVECAPTION = 2,
        INACTIVECAPTION = 3,
        MENU = 4,
        WINDOW = 5,
        WINDOWFRAME = 6,
        MENUTEXT = 7,
        WINDOWTEXT = 8,
        CAPTIONTEXT = 9,
        ACTIVEBORDER = 10,
        INACTIVEBORDER = 11,
        APPWORKSPACE = 12,
        HIGHLIGHT = 13,
        HIGHLIGHTTEXT = 14,
        BTNFACE = 15,
        BTNSHADOW = 16,
        GRAYTEXT = 17,
        BTNTEXT = 18,
        INACTIVECAPTIONTEXT = 19,
        BTNHIGHLIGHT = 20,
        DKSHADOW3D = 21,
        LIGHT3D = 22,
        INFOTEXT = 23,
        INFOBK = 24,
        LAST = INFOBK
    };


private:
    bool isAutomatic_ = false;
    union
    {
        d1::uint32 rGB_;
        AutomaticColor automaticColor_;
    };
};


const Color
    Color::Black = { 0, 0, 0 },
    Color::White = { 255, 255, 255 },
    Color::Red = { 255, 0, 0 },
    Color::Green = { 0, 255, 0 },
    Color::Blue = { 0, 0, 255 },
    Color::Yellow = { 255, 255, 0 };



inline Color::Color(AutomaticColor ac):
    isAutomatic_{ true },
    automaticColor_{ ac }
{
}

inline Color::Color(d1::uint8 red, d1::uint8 green, d1::uint8 blue):
    rGB_((blue << 16) | (green << 8) | red)
{
}

inline Color::Color(d1::uint32 color_bitarray):
    rGB_(color_bitarray)
{
    D1_ASSERT((0xFF000000 & rGB_) == 0);
}

inline d1::uint32 Color::getRGB() const
{
    D1_ASSERT(not isAutomatic_);
    return rGB_;
}

inline Color::AutomaticColor Color::getAutomaticColor() const
{
    D1_ASSERT(isAutomatic_);
    return automaticColor_;
}

inline bool Color::isAutomatic() const
{
    return isAutomatic_;
}

inline Color::Color(const Color& c):
    isAutomatic_{ c.isAutomatic_ }
{
    if (isAutomatic_)
        automaticColor_ = c.automaticColor_;
    else
        rGB_ = c.rGB_;
}

inline Color& Color::operator=(const Color& c)
{
    isAutomatic_ = c.isAutomatic_;

    if (isAutomatic_)
        automaticColor_ = c.automaticColor_;
    else
        rGB_ = c.rGB_;

    return *this;
}

inline bool Color::operator==(const Color& c) const
{
    if (isAutomatic_)
        return c.isAutomatic_ and (c.automaticColor_ == automaticColor_);
    else
        return not c.isAutomatic_ and (c.rGB_ == rGB_);
}

inline bool Color::operator<(const Color& c) const
{
    if (isAutomatic_ != c.isAutomatic_)
        return isAutomatic_ < c.isAutomatic_;

    if (isAutomatic_)
        return automaticColor_ < c.automaticColor_;

    return rGB_ < c.rGB_;
}


}
