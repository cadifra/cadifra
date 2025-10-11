/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <stdlib.h>

module d1.stringConvert;

import std;


namespace d1
{

auto string2wstring(const std::string s) -> std::wstring
{
    auto ws = std::wstring(s.size(), 0);

    size_t len = ::mbstowcs(
        const_cast<wchar_t*>(ws.c_str()), s.c_str(), s.size());

    if (len == -1)
        return L"";

    ws.resize(len);

    return ws;
}


auto wstring2string(const std::wstring ws) -> std::string
{
    auto s = std::string(2 * ws.size(), 0);

    size_t len = ::wcstombs(
        const_cast<char*>(s.c_str()), ws.c_str(), s.size());

    if (len == -1)
        return "";

    s.resize(len);

    return s;
}

}
