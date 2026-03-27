/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <math.h>

module GraphUtil.Shapes;


namespace GraphUtil
{

auto Ellipse::findNearestPoint(const d1::fPoint& p, bool horizontal) const
    -> NearestRes
{
    const d1::float64 a = rect_.width() / 2.0;
    const d1::float64 b = rect_.height() / 2.0;

    auto m = d1::vector(rect_.l + a, rect_.b + b);

    const auto r = d1::vector(p.x, p.y) - m;

    auto s = r;

    if (horizontal)
    {
        if (s.dy >= b)
        {
            s.dx = 0;
            s.dy = b;
        }
        else if (s.dy <= -b)
        {
            s.dx = 0;
            s.dy = -b;
        }
        else
        {
            const d1::float64 t = s.dy / b;
            s.dx = a * sqrt(1 - t * t);
            if (r.dx < 0)
                s.dx = -s.dx;
        }
    }
    else
    {
        if (s.dx >= a)
        {
            s.dx = a;
            s.dy = 0;
        }
        else if (s.dx <= -a)
        {
            s.dx = -a;
            s.dy = 0;
        }
        else
        {
            const d1::float64 t = s.dx / a;
            s.dy = b * sqrt(1 - t * t);
            if (r.dy < 0)
                s.dy = -s.dy;
        }
    }

    return { d1::fPoint{ m + s }, s };
}


d1::fPoint Ellipse::findNewPoint(const Ellipse& old_ellipse, const d1::fPoint& old_p) const
{
    auto old_w = old_ellipse.rect_.width();
    auto w = rect_.width();

    auto old_h = old_ellipse.rect_.height();
    auto h = rect_.height();

    auto old_origin = old_ellipse.rect_.bottomLeft();
    auto origin = rect_.bottomLeft();

    auto old_s = old_p - old_origin;

    auto s = d1::vector(old_s.dx * (w / old_w), old_s.dy * (h / old_h));

    return origin + s;
}


}
