/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.ICustomDrawer;

import Canvas.DeviceContext;

import d1.Rect;


namespace Canvas
{

export class ICustomDrawer
{
public:
    virtual ~ICustomDrawer() = default;

    virtual void doDraw(IDeviceContext&) = 0;
    virtual void doDraw(IDeviceContextStatic&) = 0;

    virtual d1::fnRect getBounds() = 0;
};

}
