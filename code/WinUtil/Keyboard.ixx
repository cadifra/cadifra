/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.Keyboard;

export namespace WinUtil::Keyboard
{

// returns true, if the control key is in downstate
bool controlIsDown()
{
    SHORT keystate = GetKeyState( // windows API function
                                  //  int nVirtKey   // virtual-key code
        VK_CONTROL);

    return (keystate & 0x80) > 0;
}


// returns true, if a shift key is in downstate
bool shiftIsDown()
{
    SHORT keystate = GetKeyState( // windows API function
                                  //  int nVirtKey   // virtual-key code
        VK_SHIFT);

    return (keystate & 0x80) > 0;
}

}
