/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.Point;

import d1.Rect;


namespace d1
{

bool Point::isNear(const Point& rhs, int32 distance) const
{
    auto r = nRect{ *this, *this };
    r.enlarge(distance);
    return r.encloses(rhs);
}


}
