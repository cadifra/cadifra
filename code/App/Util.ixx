/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.Util;

import std;


namespace App
{

export std::wstring createFileTitle(const std::wstring& filename);
// See OS function GetFileTitle.

}

module : private;

import d1.buffer;


namespace App
{

std::wstring createFileTitle(const std::wstring& filename)
{
    const int maxFileName = 2000;
    const wchar_t* invalidFileName = L"invalid";

    if (filename.size() > maxFileName)
        return invalidFileName;

    if (filename.size() == 0)
        return invalidFileName;

    auto buf = d1::wbuffer(filename.size() + 1);

    short res = ::GetFileTitle(filename.c_str(), &*buf.begin(), (WORD)buf.size());

    if (res == 0)
    {
        buf.resize(wcslen(buf.c_str()));
        return buf;
    }

    if (res < 0)
        return invalidFileName;

    if (res > maxFileName)
        return invalidFileName;

    buf.resize(res);

    res = ::GetFileTitle(filename.c_str(), &*buf.begin(), (WORD)buf.size());

    if (res == 0)
    {
        buf.resize(wcslen(buf.c_str()));
        return buf;
    }

    return invalidFileName;
}

}
