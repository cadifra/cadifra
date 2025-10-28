/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Size;

export import d1.types;

import d1.isEqual;


namespace d1
{

export struct Size
{
    using value_type = int32;

    value_type w{}, h{};

    bool operator==(const Size&) const = default;

    auto& operator+=(const Size& delta)
    {
        w += delta.w;
        h += delta.h;
        return *this;
    }
};


export struct fSize
{
    using value_type = float64;

    value_type w{}, h{};

    fSize()
    {
    }

    fSize(value_type w, value_type h):
        w{ w }, h{ h }
    {
    }

    explicit fSize(int32 w, int32 h):
        w{ static_cast<value_type>(w) }, 
        h{ static_cast<value_type>(h) }
    {
    }


    bool operator==(const fSize& rhs) const
    {
        return isEqual(w, rhs.w) && isEqual(h, rhs.h);
    }
};



export auto operator+(const Size& s1, const Size& s2)
{
    return Size{ s1.w + s2.w, s1.h + s2.h };
}

}
