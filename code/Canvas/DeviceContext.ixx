/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <gdiplus.h>

export module Canvas.DeviceContext;

import d1.Point;

import FontUtil;


namespace Canvas
{

export class IDeviceContext
{
public:
    virtual HDC dangerousAccess() = 0;

    virtual void setTextAlign(UINT mode) = 0;
    virtual void setTextColor(COLORREF) = 0;
    virtual void setBkColor(COLORREF) = 0;

    virtual d1::fVector offset() const = 0;
    virtual d1::fPoint scale() const = 0;

    virtual void selectFont(const FontUtil::FontRef&) = 0;

protected:
    ~IDeviceContext() = default;
};


export class IDeviceContextStatic
{
public:
    virtual Gdiplus::Graphics* getGraphics() { return 0; }

    virtual HDC dangerousAccess() = 0;

    virtual void setTextAlign(UINT mode) = 0;
    virtual void setTextColor(COLORREF) = 0;
    virtual void setBkColor(COLORREF) = 0;

    virtual d1::fVector offset() const = 0;
    virtual d1::fPoint scale() const = 0;

    virtual void selectFont(const LOGFONT&) = 0;

protected:
    ~IDeviceContextStatic() = default;
};

}
