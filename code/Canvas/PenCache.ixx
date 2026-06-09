/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module Canvas.PenCache;

import Canvas.GdiObj;

import WinUtil.types;

import std;


export namespace Canvas::PenCache
{

auto Default() -> GdiObj<WinUtil::HPEN>;
auto marker() -> GdiObj<WinUtil::HPEN>;
auto blackDashed() -> GdiObj<WinUtil::HPEN>;
auto alternate() -> GdiObj<WinUtil::HPEN>;
auto null() -> GdiObj<WinUtil::HPEN>;

}

module : private;


namespace Canvas::PenCache
{

constexpr COLORREF Black = RGB(0, 0, 0);

struct Logbrush: public LOGBRUSH
{
    Logbrush(COLORREF c = Black)
    {
        lbStyle = BS_SOLID;
        lbColor = c;
        lbHatch = 0;
    }
};


auto Default() -> GdiObj<HPEN>
{
    Logbrush logb;
    static auto p = GdiObj<HPEN>{ ::ExtCreatePen(
        PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_ROUND,
        1,
        &logb,
        0,
        0) };
    D1_ASSERT(p);
    return p;
}


auto marker() -> GdiObj<HPEN>
{
    static auto p = GdiObj<HPEN>{ ::CreatePen(PS_DOT, 1, RGB(200, 10, 10)) };
    D1_ASSERT(p);
    return p;
}


auto blackDashed() -> GdiObj<HPEN>
{
    static auto p = GdiObj<HPEN>{ ::CreatePen(PS_DOT, 1, Black) };
    D1_ASSERT(p);
    return p;
}


auto alternate() -> GdiObj<HPEN>
{
    auto logb = Logbrush(RGB(150, 175, 150));
    static auto p = GdiObj<HPEN>{ ::ExtCreatePen(
        PS_COSMETIC | PS_ALTERNATE,
        1,
        &logb,
        0,
        0) };
    D1_ASSERT(p);
    return p;
}


auto null() -> GdiObj<HPEN>
{
    static auto p = GdiObj<HPEN>{ ::CreatePen(PS_NULL, 1, Black) };
    D1_ASSERT(p);
    return p;
}

}
