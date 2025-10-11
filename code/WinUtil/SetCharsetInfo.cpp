/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;

import std;


namespace WinUtil
{

bool FromCharset(CHARSETINFO& ci, DWORD charset)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(charset)),
                    &ci, TCI_SRCCHARSET);
}


bool FromCodepage(CHARSETINFO& ci, DWORD codepage)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(codepage)),
                    &ci, TCI_SRCCODEPAGE);
}


bool FromLCID(CHARSETINFO& ci, LCID lcid)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(lcid)),
                    &ci, TCI_SRCLOCALE);
}


bool FromLANGID(CHARSETINFO& ci, LANGID langid)
{
    ci = {};
    return 0 != ::TranslateCharsetInfo(
                    reinterpret_cast<DWORD*>(static_cast<std::uint64_t>(langid)),
                    &ci, TCI_SRCLOCALE);
}


}
