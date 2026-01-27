/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
 
module WinUtil.MsgBox;

import WinUtil.CursorManager;


namespace WinUtil::MsgBox
{

int show(
    const std::wstring& title,
    const std::wstring& text,
    UINT uType)
{
    auto iwc = CursorManager::InhibitWaitCursor{};

    return ::MessageBox(
        ::GetActiveWindow(),
        text.c_str(),
        title.c_str(),
        uType);
}

}
