/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.Mouse;

import d1.MouseButton;
import d1.types;
import d1.wintypes;


namespace WinUtil
{

export class MouseInputCapturer
{
public:
    class Control;

private:
    Control& control_;
    bool isCaptured_ = false;

public:
    MouseInputCapturer(Control& mic):
        control_{ mic }
    {
    }

    MouseInputCapturer(Control& mic, bool capture);

    ~MouseInputCapturer()
    {
        release();
    }

    void capture();
    void release();

    bool isCaptured() const
    {
        return isCaptured_;
    }
};


class MouseInputCapturer::Control
{
    friend class MouseInputCapturer;

    d1::int32 numCapture_ = 0;

private:
    void captureMouseInput();
    // After calling CaptureMouseInput for the active Window, all mouse
    // input messages are delivered to the active window, even if the hot spot
    // is outside the window (normally, the active window receives only mouse
    // messages with hot spots inside the active window.
    // Calling releaseMouseInput() reverts to normal behavior, as it was
    // before calling captureMouseInput().
    // Every call to CaptureMouseInput must be balanced by a call to
    // ReleaseMouseInput.

    void releaseMouseInput();

protected:
    ~Control() = default;

private:
    virtual void implSetCaptureMouseInput(bool) = 0;
};


MouseInputCapturer::MouseInputCapturer(Control& mic, bool capture):
    control_{ mic }, isCaptured_{ capture }
{
    if (isCaptured_)
        control_.captureMouseInput();
}


void MouseInputCapturer::capture()
{
    if (not isCaptured_)
    {
        control_.captureMouseInput();
        isCaptured_ = true;
    }
}


void MouseInputCapturer::release()
{
    if (isCaptured_)
    {
        control_.releaseMouseInput();
        isCaptured_ = false;
    }
}


export namespace MouseButton
{

d1::WPARAM fwKeysVal(d1::MouseButton mb);

}


void MouseInputCapturer::Control::captureMouseInput()
{
    if (numCapture_++ == 0)
        this->implSetCaptureMouseInput(true);
}


void MouseInputCapturer::Control::releaseMouseInput()
{
    if (numCapture_ > 0)
    {
        if (--numCapture_ == 0)
            this->implSetCaptureMouseInput(false);
    }
}


d1::WPARAM MouseButton::fwKeysVal(d1::MouseButton mb)
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
