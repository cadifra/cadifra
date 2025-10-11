/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil:Mouse;

import d1.MouseButton;
import d1.types;


namespace WinUtil
{

export class IMouseInputControl
{
    friend class MouseInputCapturer;

private:
    d1::int32 itsNumCapture = 0;

private:
    void CaptureMouseInput();
    // After calling CaptureMouseInput for the active Window, all mouse
    // input messages are delivered to the active window, even if the hot spot
    // is outside the window (normally, the active window receives only mouse
    // messages with hot spots inside the active window.
    // Calling ReleaseMouseInput() reverts to normal behavior, as it was
    // before calling CaptureMouseInput().
    // Every call to CaptureMouseInput must be balanced by a call to
    // ReleaseMouseInput.

    void ReleaseMouseInput();

protected:
    ~IMouseInputControl() = default;

private:
    virtual void ImplSetCaptureMouseInput(bool) = 0;
};


export class MouseInputCapturer
{
    IMouseInputControl& itsMouseInputControl;
    bool itIsCaptured = false;

public:
    MouseInputCapturer(IMouseInputControl& mic):
        itsMouseInputControl{ mic }
    {
    }

    MouseInputCapturer(IMouseInputControl& mic, bool capture):
        itsMouseInputControl{ mic }, itIsCaptured{ capture }
    {
        if (itIsCaptured)
            itsMouseInputControl.CaptureMouseInput();
    }

    ~MouseInputCapturer()
    {
        Release();
    }

    void Capture()
    {
        if (!itIsCaptured)
        {
            itsMouseInputControl.CaptureMouseInput();
            itIsCaptured = true;
        }
    }

    void Release()
    {
        if (itIsCaptured)
        {
            itsMouseInputControl.ReleaseMouseInput();
            itIsCaptured = false;
        }
    }

    bool IsCaptured() const
    {
        return itIsCaptured;
    }
};


export namespace MouseButton
{

WPARAM fwKeysVal(d1::MouseButton mb)
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
