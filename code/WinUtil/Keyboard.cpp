/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;


namespace WinUtil::Keyboard
{

bool ControlIsDown()
{
    SHORT keystate = GetKeyState( // windows API function
                                  //  int nVirtKey   // virtual-key code
        VK_CONTROL);

    return (keystate & 0x80) > 0;
}


bool ShiftIsDown()
{
    SHORT keystate = GetKeyState( // windows API function
                                  //  int nVirtKey   // virtual-key code
        VK_SHIFT);

    return (keystate & 0x80) > 0;
}

}

