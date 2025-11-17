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
    HDC itsDC = {};        // no ownership
    HGDIOBJ itsOld = {};   // no ownership
    T itsSelected = { 0 }; // no ownership

public:
    GdiObjectSelector(HDC dc):
        itsDC{ dc }
    {
    }

    ~GdiObjectSelector() { Unselect(); }

    HDC Select(T t);
    void Unselect(T t);
    void Unselect() { Unselect(itsSelected); }

    GdiObjectSelector(const GdiObjectSelector&) = delete;
    GdiObjectSelector& operator=(const GdiObjectSelector&) = delete;
};



template <class T>
HDC GdiObjectSelector<T>::Select(T t)
{
    if (itsSelected == t || t == 0)
        return itsDC;

    HGDIOBJ old = ::SelectObject(itsDC, t);
    if (itsOld == 0)
        itsOld = old;

    itsSelected = t;
    return itsDC;
}


template <class T>
void GdiObjectSelector<T>::Unselect(T t)
{
    if (itsSelected != t || t == 0)
        return;

    ::SelectObject(itsDC, itsOld);
    itsSelected = 0;
}

export class GdiplusException: public d1::Exception
{
public:
    //-- std::exception

    const char* what() const override;

    //--

    static void Check(Gdiplus::Status s)
    {
        if (s == Gdiplus::Win32Error)
            throw ComException(::GetLastError());

        if (s != Gdiplus::Ok)
            throw GdiplusException(s);
    }

    GdiplusException(Gdiplus::Status s):
        itsStatus{ s }
    {
    }

    Gdiplus::Status GetStatus() const { return itsStatus; }

    // uses compiler generated copy ctor and assignment operator

private:
    Gdiplus::Status itsStatus;
};

}
