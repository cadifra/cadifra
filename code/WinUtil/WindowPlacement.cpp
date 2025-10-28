/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Window;


namespace WinUtil
{

WindowPlacement::WindowPlacement():
    WINDOWPLACEMENT{}
{
    length = sizeof(WINDOWPLACEMENT);
}

}

