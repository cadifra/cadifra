/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <math.h>

export module Canvas.Dashes;

import d1.types;

export namespace Canvas::Dashes
{

constexpr d1::float64 DASH = 200.0;  // 2mm
constexpr d1::float64 SPACE = 100.0; // 1mm


d1::float64 offset(const d1::float64& length);
// Offset calculates the offset for drawing a dashed line with
// the length "length". You have to start the line with a dash.
// This first dash must have a length of "DASH - Offset" to get
// good looking results.

}

module : private;


namespace Canvas::Dashes
{

constexpr d1::float64 P = DASH + SPACE;


d1::float64 offset(const d1::float64& length)
{
    if (length <= DASH)
        return 0;

    const d1::float64 L = length - DASH;

    return 0.5 * (ceil(L / P) * P - L);
}

}
