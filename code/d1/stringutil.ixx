/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.stringutil;

import std;


export namespace d1
{

inline size_t strnlen(const char* p, size_t max)
{
    size_t n = 0;
    while (n < max && (*p != 0))
    {
        ++n;
        ++p;
    }
    return n;
}


inline size_t strnlen(const wchar_t* p, size_t max)
{
    size_t n = 0;
    while (n < max && (*p != 0))
    {
        ++n;
        ++p;
    }
    return n;
}


inline std::string Cstring2string(const char* p, size_t max)
{
    const auto res = std::string(p, strnlen(p, max));
    return res;
}


inline std::wstring Cstring2string(const wchar_t* p, size_t max)
{
    const auto res = std::wstring(p, strnlen(p, max));
    return res;
}


int replace(
    std::wstring& in, const std::wstring& what, const std::wstring& with);


}
