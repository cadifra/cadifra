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
    Control& itsControl;
    bool itIsCaptured = false;

public:
    MouseInputCapturer(Control& mic):
        itsControl{ mic }
    {
    }

    MouseInputCapturer(Control& mic, bool capture);

    ~MouseInputCapturer()
    {
        Release();
    }

    void Capture();
    void Release();

    bool IsCaptured() const
    {
        return itIsCaptured;
    }
};


class MouseInputCapturer::Control
{
    friend class MouseInputCapturer;

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
    ~Control() = default;

private:
    virtual void ImplSetCaptureMouseInput(bool) = 0;
};


MouseInputCapturer::MouseInputCapturer(Control& mic, bool capture):
    itsControl{ mic }, itIsCaptured{ capture }
{
    if (itIsCaptured)
        itsControl.CaptureMouseInput();
}


void MouseInputCapturer::Capture()
{
    if (!itIsCaptured)
    {
        itsControl.CaptureMouseInput();
        itIsCaptured = true;
    }
}


void MouseInputCapturer::Release()
{
    if (itIsCaptured)
    {
        itsControl.ReleaseMouseInput();
        itIsCaptured = false;
    }
}


export namespace MouseButton
{

d1::WPARAM fwKeysVal(d1::MouseButton mb);

}

}
