/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

/*
  Below you will find a call to the horrible ::getOpenFileName(),
  which is a still a real pain to use.

  GetOpenFileName() has the following bug:

  If the user selects multiple files, the state of the "readonly" checkbox
  flag OFN_READONLY is always not set, no matter whether the user checked
  the readonly checkbox or not.

  To solve this, we provide a hook function to check for the nasty
  readonly checkbox ourselves.
*/

module;

#include "d1/d1assert.h"

#include <Windows.h>
#include <dlgs.h>

module WinUtil.OpenFileDialog;

import WinUtil.CursorManager;
import WinUtil.ExceptionBox;
import WinUtil.Window;

import d1.buffer;


import std;


namespace WinUtil
{

namespace
{

using StringVector = std::vector<std::wstring>;

class FileNamesBuffer
{
    std::vector<TCHAR> buf_;

public:
    FileNamesBuffer(DWORD size):
        buf_(size)
    {
    }

    LPTSTR ptr() { return buf_.data(); }
    DWORD size() const { return static_cast<DWORD>(buf_.size()); }

    enum class ParseResult
    {
        OK = 0,
        UNEXPECTED_BUFFER_END = -1
    };

    ParseResult parse(StringVector& v) const
    {
        auto p = begin(buf_);
        auto buf_end = end(buf_);

        while (*p != 0)
        {
            auto a = p;
            while (*p != 0)
            {
                ++p;
                if (p == buf_end)
                    return ParseResult::UNEXPECTED_BUFFER_END;
            }
            D1_ASSERT(p != a);
            v.push_back(std::wstring(a, p));
            ++p;
            if (p == buf_end)
                return ParseResult::UNEXPECTED_BUFFER_END;
        }
        return ParseResult::OK;
    }
};


inline bool containsPath(const std::wstring& s)
{
    return (s.find(L'\\') != std::string::npos);
}


inline bool hasBackslashAtEnd(const std::wstring& s)
{
    return not s.empty() and (*rbegin(s) == L'\\');
}

struct HookInfo
{
    bool readOnly = false;
};

UINT APIENTRY DialogHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NOTIFY)
    {
        LPOFNOTIFY lpofn = reinterpret_cast<LPOFNOTIFY>(lParam);

        if (lpofn->hdr.code == CDN_FILEOK)
        {
            HookInfo* hi = reinterpret_cast<HookInfo*>(lpofn->lpOFN->lCustData);
            HWND parent = GetParent(hwnd);

            hi->readOnly =
                (BST_CHECKED == ::IsDlgButtonChecked(parent, chx1));
        }
    }
    return (0);
}

}


OpenFileDialog::OpenFileDialog(
    HWND owner,
    const std::wstring& filter,
    const Flags* flags,
    const std::wstring* title)
{
    if (not flags)
    {
        static const Flags default_flags;
        flags = &default_flags;
    }

    auto iwc = CursorManager::ImmediateWaitCursor{};
    auto wd = WindowDisabler{};

    auto filterbuf = std::wstring(filter);
    filterbuf += d1::wbuffer(2);

    HookInfo hookInfo;

    auto of = OPENFILENAME{};

    FileNamesBuffer fnames(10 * 1024); // allocate 20kB for file name buffer
    // FileNamesBuffer fnames(256); // for testing: deliberately small to test error case

    of.lStructSize = sizeof(OPENFILENAME);
    of.hwndOwner = owner;
    of.hInstance = 0; // ignored
    of.lpstrFilter = filterbuf.c_str();
    of.lpstrCustomFilter = 0;
    of.nMaxCustFilter = 0; // ignored
    of.nFilterIndex = 0;

    of.lpstrFile = fnames.ptr();
    of.nMaxFile = fnames.size();

    of.lpstrFileTitle = 0;
    of.nMaxFileTitle = 0;   // ignored
    of.lpstrInitialDir = 0; // use current directory
    of.lpstrTitle = title ? title->c_str() : 0;
    of.Flags =
        (flags->AllowMultiselect ? OFN_ALLOWMULTISELECT : 0) |
        (flags->HideReadOnly ? OFN_HIDEREADONLY : 0) |
        OFN_EXPLORER |
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST |
        OFN_ENABLESIZING |
        OFN_ENABLEHOOK;

    of.nFileOffset = 0; // ignored, valid on return
    of.nFileExtension = 0;
    of.lpstrDefExt = 0;
    of.lCustData = reinterpret_cast<LPARAM>(&hookInfo);
    of.lpfnHook = reinterpret_cast<LPOFNHOOKPROC>(DialogHook);
    of.lpTemplateName = 0;


    if (::GetOpenFileName(&of) != TRUE)
    {
        const DWORD error = ::CommDlgExtendedError();

        if (error == 0)
        {
            // ## user canceled or closed dialog box -> normal use case
            return;
        }

        auto os = std::ostream(&WinUtil::ExceptionBox::clear());
        os
            << "error calling ::GetOpenFileName() in WinUtil::OpenFileDialog" << '\n'
            << "error code = 0x" << std::hex << error << '\n';
        ExceptionBox::show();
        return; // failed
    }

    readOnly = hookInfo.readOnly;


    auto sv = StringVector{};
    auto parse_res = fnames.parse(sv);
    if (parse_res == FileNamesBuffer::ParseResult::UNEXPECTED_BUFFER_END)
    {
        if (sv.size() < 2)
            // ## we couldn't even get the directory plus one file -> inacceptable
            return;
        else
            // ## despite we ran into an unexpected buffer end, we do have
            // ## at least two Null terminated entries. Let's be graceful and
            // ## continue with what we have (at least for the release build).
            D1_ASSERT(0);
    }
    else if (parse_res != FileNamesBuffer::ParseResult::OK)
        return; // other parse error


    if (sv.empty())
        return; // nothing selected;

    if (sv.size() == 1)
    {
        // ## only one file selected, together with path
        const auto file = std::wstring(*begin(sv));
        fileList.insert(file);
        isOk = true;
        return;
    }

    // ## multiple selection

    std::wstring directory_path;

    for (bool first = true; const std::wstring& s : sv)
    {
        if (first)
        {
            // ## this is the first string in the list. It's the path of the
            // ## directory.
            directory_path = s;
            first = false;
        }
        else if (containsPath(s))
        {
            // ## s contains a path character (backslash).
            //  In this case we must ignore directory_path. This situation occurs
            //  if the user selected a shortcut (which is automatically converted to
            //  its target's path and filename).
            fileList.insert(s);
        }
        else
        {
            // ## s contains _no_ path character (backslash).
            const std::wstring file = (hasBackslashAtEnd(directory_path)
                                           ? directory_path + s
                                           : directory_path + L'\\' + s);
            fileList.insert(file);
        }
    }

    isOk = true;
}

}
