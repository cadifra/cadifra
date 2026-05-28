/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>
#include <commdlg.h>

export module WinUtil.SaveFileDialog;

import d1.wintypes;
import d1.buffer;

import WinUtil.CursorManager;
import WinUtil.WindowDisabler;

import std;


constexpr int BufferSize = 1024;


export namespace WinUtil::SaveFileDialog
{

bool show(d1::HWND owner, const std::wstring& filter, int filterIndex,
    const std::wstring& defaultExtension, std::wstring& fileName)

// Example for filter: "Code Files (*.h;*.cpp)\0*.h;*.cpp\0All Files\0*\0"
// filterIndex==1: the first filter entry is selected.

{
    auto iwc = CursorManager::ImmediateWaitCursor{};
    auto wd = WindowDisabler{};

    auto filterbuf = std::wstring(filter);
    filterbuf += d1::wbuffer(2);

    D1_ASSERT(fileName.size() <= BufferSize);
    fileName.resize(BufferSize);

    auto of = OPENFILENAME{};

    of.lStructSize = sizeof(of);
    of.hwndOwner = owner;
    of.lpstrFilter = filterbuf.c_str();
    of.nFilterIndex = filterIndex;
    of.lpstrFile = fileName.data();
    of.nMaxFile = static_cast<DWORD>(fileName.size());
    of.lpstrDefExt = defaultExtension.c_str();
    of.Flags =
        OFN_ENABLESIZING |
        OFN_EXPLORER |
        OFN_PATHMUSTEXIST |
        OFN_NOREADONLYRETURN |
        OFN_HIDEREADONLY |
        OFN_OVERWRITEPROMPT;


    if (::GetSaveFileName(&of))
    {
        fileName.resize(wcslen(fileName.c_str()));
        return true;
    }
    else
    {
        DWORD err = ::CommDlgExtendedError();
        D1_ASSERT(not err);
        return false;
    }
}

}
