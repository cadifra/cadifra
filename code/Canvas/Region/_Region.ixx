/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Region;

import d1.Point;
import d1.wintypes;

import WinUtil.types;


namespace Canvas
{
// Region is a mapper for the region object of the operating system

export class Region
{
    WinUtil::HRGN rgn_ = {};

public:
    Region(WinUtil::HRGN rgn):
        rgn_(rgn) {} // takes ownership of rgn

    Region();                // creates a NULLREGION
    Region(const d1::RECT&); // creates a rectangular region (SIMPLEREGION)

    ~Region(); // intentionally not virtual

    Region(const Region&);
    Region& operator=(const Region&);

    WinUtil::HRGN val() const { return rgn_; }

    bool operator==(const Region&);

    Region& operator+=(const Region&);
    Region& operator-=(const Region&);
    Region& operator*=(const Region&);

    Region& translate(const d1::Point& offset);


    friend Region operator+(const Region&, const Region&);
    friend Region operator-(const Region&, const Region&);
    friend Region operator*(const Region&, const Region&);
};

}
