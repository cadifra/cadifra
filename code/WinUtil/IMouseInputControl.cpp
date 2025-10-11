/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module WinUtil;


namespace WinUtil
{

namespace
{
using C = IMouseInputControl;
}


void C::CaptureMouseInput()
{
    if (itsNumCapture++ == 0)
        this->ImplSetCaptureMouseInput(true);
}


void C::ReleaseMouseInput()
{
    if (itsNumCapture > 0)
    {
        if (--itsNumCapture == 0)
            this->ImplSetCaptureMouseInput(false);
    }
}

}
