/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>
#include <commdlg.h>

module WinUtil.SaveFileDialog;

import WinUtil.CursorManager;
import WinUtil.Window;

import d1.buffer;


namespace
{
constexpr int BufferSize = 1024;
}

namespace WinUtil::SaveFileDialog
{

bool Show(HWND owner, const std::wstring& filter, int filterIndex,
    const std::wstring& defaultExtension, std::wstring& fileName)
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
        D1_ASSERT(!err);
        return false;
    }
}

}
