/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.RectOstream;

import d1.Rect;

import std;


namespace d1
{

export inline auto& operator<<(std::ostream& s, const nRect& r)
{
    s << "(L=" << r.left()
      << ",T=" << r.top()
      << ",R=" << r.right()
      << ",B=" << r.bottom()
      << ")";
    return s;
}

}
