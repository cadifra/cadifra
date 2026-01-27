/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

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

}
