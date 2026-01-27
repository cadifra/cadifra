/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <gdiplus.h>

#include "d1/d1verify.h"

export module WinUtil.Gdi;

import d1.Exception;

import WinUtil.ComException;

import std;


namespace WinUtil
{

export template <class T> // HFONT, HPEN etc.
struct GdiObjectOwnerDel
{
    using pointer = T;

    void operator()(T h)
    {
        D1_VERIFY(::DeleteObject(h));
    }
};

export template <class T>
using GdiObjectOwner = std::unique_ptr<T, GdiObjectOwnerDel<T>>;


export template <class T> // HFONT, HPEN etc.
class GdiObjectSelector
{
    HDC DC_ = {};        // no ownership
    HGDIOBJ old_ = {};   // no ownership
    T selected_ = { 0 }; // no ownership

public:
    GdiObjectSelector(HDC dc):
        DC_{ dc }
    {
    }

    ~GdiObjectSelector() { unselect(); }

    HDC select(T t);
    void unselect(T t);
    void unselect() { unselect(selected_); }

    GdiObjectSelector(const GdiObjectSelector&) = delete;
    GdiObjectSelector& operator=(const GdiObjectSelector&) = delete;
};



template <class T>
HDC GdiObjectSelector<T>::select(T t)
{
    if (selected_ == t or t == 0)
        return DC_;

    HGDIOBJ old = ::SelectObject(DC_, t);
    if (old_ == 0)
        old_ = old;

    selected_ = t;
    return DC_;
}


template <class T>
void GdiObjectSelector<T>::unselect(T t)
{
    if (selected_ != t or t == 0)
        return;

    ::SelectObject(DC_, old_);
    selected_ = 0;
}

export class GdiplusException: public d1::Exception
{
public:
    //-- std::exception

    const char* what() const override;

    //--

    static void check(Gdiplus::Status s)
    {
        if (s == Gdiplus::Win32Error)
            throw ComException(::GetLastError());

        if (s != Gdiplus::Ok)
            throw GdiplusException(s);
    }

    GdiplusException(Gdiplus::Status s):
        status_{ s }
    {
    }

    Gdiplus::Status GetStatus() const { return status_; }

    // uses compiler generated copy ctor and assignment operator

private:
    Gdiplus::Status status_;
};

}
