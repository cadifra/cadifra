/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:d1DragDetect;

import d1.MouseButton;
import d1.wintypes;


namespace WinUtil
{

export bool d1DragDetect(
    d1::HWND wnd, // handle to window
    d1::POINT pt, // initial position
    d1::MouseButton mb = d1::MouseButton::left);
// replaces Win API function DragDetect, which works only for
// left mouse button. Detect tries to do the same as ::DragDetect,
// but for the other buttons too.

}
