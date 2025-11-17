/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.Point;

import d1.Rect;


namespace d1
{

bool Point::IsNear(const Point& rhs, int32 distance) const
{
    auto r = nRect{ *this, *this };
    r.Enlarge(distance);
    return r.Encloses(rhs);
}


}
