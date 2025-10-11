/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:SaveFileDialog;

import d1.wintypes;

import std;


export namespace WinUtil::SaveFileDialog
{

bool Show(d1::HWND owner, const std::wstring& filter, int filterIndex,
    const std::wstring& defaultExtension, std::wstring& fileName);

// Example for filter: "Code Files (*.h;*.cpp)\0*.h;*.cpp\0All Files\0*\0"
// filterIndex==1: the first filter entry is selected.

}
