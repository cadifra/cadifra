/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil.Messages;

import d1.Point;


namespace WinUtil
{

namespace
{
using C = WM_CONTEXTMENU_Msg;
}


d1::Point C::getClientCoordinates() const
{
    auto p = POINT{
        .x = xPos(),
        .y = yPos()
    };

    D1_VERIFY(::ScreenToClient(hwnd(), &p));

    return { p.x, p.y };
}


bool C::isKeyboardGenerated() const
{
    return xPos() == -1 and yPos() == -1;
    // According to MSDN documentation, message is caused by keyboard, not
    // by right mouse button.
    // Note: This is incompatible with multiple monitors, as the virtual
    // screen used with multiple monitors uses negative virtual screens
    // coordinates. The MSDN documentation fails to state what will happen
    // with multiple screen coordinates.
}

}
