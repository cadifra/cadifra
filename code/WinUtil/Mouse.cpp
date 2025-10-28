/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.Mouse;


namespace WinUtil
{

void MouseInputCapturer::Control::CaptureMouseInput()
{
    if (itsNumCapture++ == 0)
        this->ImplSetCaptureMouseInput(true);
}


void MouseInputCapturer::Control::ReleaseMouseInput()
{
    if (itsNumCapture > 0)
    {
        if (--itsNumCapture == 0)
            this->ImplSetCaptureMouseInput(false);
    }
}


namespace MouseButton
{

d1::WPARAM fwKeysVal(d1::MouseButton mb)
{
    switch (mb)
    {
    case d1::MouseButton::left:
        return MK_LBUTTON;
    case d1::MouseButton::middle:
        return MK_MBUTTON;
    case d1::MouseButton::right:
        return MK_RBUTTON;
    default:
        D1_ASSERT(0);
        return 0;
    }
}

}

}
