/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <math.h>

module Canvas.Dashes;


namespace Canvas::Dashes
{

namespace
{
constexpr d1::float64 P = DASH + SPACE;
}


d1::float64 offset(const d1::float64& length)
{
    if (length <= DASH)
        return 0;

    const d1::float64 L = length - DASH;

    return 0.5 * (ceil(L / P) * P - L);
}

}
