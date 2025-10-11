/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Range;

namespace d1
{

template <typename T>
class _Range
{
    T itsLow;
    T itsHigh;

public:
    _Range(T l, T h):
        itsLow{ l }, itsHigh{ h }
    {
        if (l > h)
        {
            itsLow = h;
            itsHigh = l;
        }
    }

    _Range Limit(auto& v) const
    {
        if (v < itsLow)
            v = itsLow;
        if (v > itsHigh)
            v = itsHigh;
        return *this;
    }
};


export template <typename T>
auto Range(T l, T h)
{
    return _Range{ l, h };
}

}
