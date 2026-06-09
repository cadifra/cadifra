/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

export module Canvas.GdiObj;

import WinUtil.types;

import std;


namespace Canvas
{

export template <class T> // T is HPEN, HBRUSH, HFONT, HBITMAP or HRGN
class GdiObj
{
    std::shared_ptr<std::remove_pointer_t<T>> sharedPtr_;

    struct Del;

public:
    GdiObj()
    {
    }

    GdiObj(std::nullptr_t)
    {
    }

    GdiObj(T g); // takes ownership of g

    GdiObj(const GdiObj& rhs) = default;
    GdiObj& operator=(const GdiObj& rhs) = default;

    operator T() const { return static_cast<T>(sharedPtr_.get()); }
};


template <class T>
struct GdiObj<T>::Del
{
    void operator()(T h) const
    {
        if (h)
            D1_VERIFY(::DeleteObject(h));
    }
};


template <class T>
GdiObj<T>::GdiObj(T g):
    sharedPtr_{ g, Del{} }
{
}

}
