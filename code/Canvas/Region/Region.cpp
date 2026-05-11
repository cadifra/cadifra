/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module Canvas.Region;

import d1.Point;


namespace Canvas
{

namespace
{
using C = Region;
}


C::Region()
{
    rgn_ = ::CreateRectRgn(0, 0, 0, 0);
    D1_ASSERT(rgn_);
}


C::Region(const RECT& r)
{
    rgn_ = ::CreateRectRgnIndirect(&r);
    D1_ASSERT(rgn_);
}


C::~Region()
{
    if (rgn_)
        D1_VERIFY(::DeleteObject(rgn_));
}


C::Region(const Region& rhs)
{
    rgn_ = ::CreateRectRgn(0, 0, 0, 0);
    D1_ASSERT(rgn_);
    D1_VERIFY(ERROR != ::CombineRgn(
                           rgn_,
                           rhs.rgn_,
                           0,
                           RGN_COPY));
}


Region& C::operator=(const Region& rhs)
{
    if (&rhs == this)
        return *this;

    D1_VERIFY(ERROR != ::CombineRgn(
                           rgn_,
                           rhs.rgn_,
                           0,
                           RGN_COPY));

    return *this;
}


bool C::operator==(const Region& rhs)
{
    return ::EqualRgn(rgn_, rhs.rgn_) ? true : false;
}


Region& C::operator+=(const Region& rhs)
{
    D1_VERIFY(ERROR != ::CombineRgn(
                           rgn_,
                           rgn_,
                           rhs.rgn_,
                           RGN_OR));
    return *this;
}


Region& C::operator-=(const Region& rhs)
{
    D1_VERIFY(ERROR != ::CombineRgn(
                           rgn_,
                           rgn_,
                           rhs.rgn_,
                           RGN_DIFF));
    return *this;
}


Region& C::operator*=(const Region& rhs)
{
    D1_VERIFY(ERROR != ::CombineRgn(
                           rgn_,
                           rgn_,
                           rhs.rgn_,
                           RGN_AND));
    return *this;
}


Region& C::translate(const d1::Point& offset)
{
    D1_VERIFY(ERROR != ::OffsetRgn(rgn_, offset.x, offset.y));
    return *this;
}


Region operator+(const Region& lhs, const Region& rhs)
{
    HRGN res = ::CreateRectRgn(0, 0, 0, 0);
    D1_ASSERT(res);

    D1_VERIFY(ERROR != ::CombineRgn(
                           res,
                           lhs.rgn_,
                           rhs.rgn_,
                           RGN_OR));
    return { res };
}


Region operator-(const Region& lhs, const Region& rhs)
{
    HRGN res = ::CreateRectRgn(0, 0, 0, 0);
    D1_ASSERT(res);

    D1_VERIFY(ERROR != ::CombineRgn(
                           res,
                           lhs.rgn_,
                           rhs.rgn_,
                           RGN_DIFF));
    return { res };
}


Region operator*(const Region& lhs, const Region& rhs)
{
    HRGN res = ::CreateRectRgn(0, 0, 0, 0);
    D1_ASSERT(res);

    D1_VERIFY(ERROR != ::CombineRgn(
                           res,
                           lhs.rgn_,
                           rhs.rgn_,
                           RGN_AND));
    return { res };
}

}
