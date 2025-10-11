/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.round;

import std;


namespace d1
{

export inline int round(double d)
{
    return static_cast<int>(std::round(d));
}

}
