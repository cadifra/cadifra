/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1assert.h"

module WinUtil.Messages;


namespace WinUtil
{

bool WM_MOUSEMOVE_Msg::ButtonDown(d1::MouseButton mb) const
{
    const WPARAM wparam = GetWParam();
    switch (mb)
    {
    case d1::MouseButton::left:
        return (wparam & MK_LBUTTON) > 0;
    case d1::MouseButton::middle:
        return (wparam & MK_MBUTTON) > 0;
    case d1::MouseButton::right:
        return (wparam & MK_RBUTTON) > 0;
    default:
        D1_ASSERT(0);
        return false;
    }
}

}
