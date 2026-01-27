/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.SetCharsetInfo;

import std;


namespace WinUtil
{

bool fromCharset(CHARSETINFO& ci, DWORD charset)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(charset)),
                    &ci, TCI_SRCCHARSET);
}


bool fromCodepage(CHARSETINFO& ci, DWORD codepage)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(codepage)),
                    &ci, TCI_SRCCODEPAGE);
}


bool fromLCID(CHARSETINFO& ci, LCID lcid)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(lcid)),
                    &ci, TCI_SRCLOCALE);
}


bool fromLANGID(CHARSETINFO& ci, LANGID langid)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(langid)),
                    &ci, TCI_SRCLOCALE);
}


}
