/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Grid;

import d1.Rect;


namespace Core
{

export class IGrid
{
public:
    virtual d1::Point toGrid(const d1::Point& p) const = 0;
    // find nearest position on grid to.

    // enlarge to grid functions

    virtual d1::int32 enlarge(d1::int32 d) const = 0;

    virtual d1::Vector enlarge(const d1::Vector& v) const = 0;
    // if the coordinates of v are not in even grid spaces, enlarge them
    // to next multiple.

    virtual d1::Size enlarge(const d1::Size& p) const = 0;

    virtual d1::nRect enlarge(const d1::nRect&) const = 0;

protected:
    ~IGrid() = default;
};

}
