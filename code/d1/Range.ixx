/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Range;

namespace d1
{

template <typename T>
class Range
{
    T low_;
    T high_;

public:
    Range(T l, T h):
        low_{ l }, high_{ h }
    {
        if (l > h)
        {
            low_ = h;
            high_ = l;
        }
    }

    Range limit(auto& v) const
    {
        if (v < low_)
            v = low_;
        if (v > high_)
            v = high_;
        return *this;
    }
};


export template <typename T>
auto range(T l, T h)
{
    return Range{ l, h };
}

}
