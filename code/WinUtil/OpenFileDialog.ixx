/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:OpenFileDialog;

import d1.wintypes;

import std;


namespace WinUtil
{

export class OpenFileDialog
{
public:
    using FileList = std::set<std::wstring>;

    FileList fileList;
    bool readOnly = false;
    bool isOk = false;


    class Flags
    {
    public:
        bool AllowMultiselect = true;
        bool HideReadOnly = false;
    };

    OpenFileDialog(
        d1::HWND owner,
        const std::wstring& filter,
        const Flags* flags = 0,       // 0 means use defaults
        const std::wstring* title = 0 // 0 means use defaults
    );

    // Example for filter: "Code Files (*.h;*.cpp)\0*.h;*.cpp\0All Files\0*\0"
};

}
