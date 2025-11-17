/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

module d1.Path;

import std;


namespace d1
{

namespace Path
{


size_type FindExtension(const wstring& path)
{
    for (auto p = path.size(); p > 0; --p)
    {
        auto c = path[p - 1];
        if (c == L'\0')
            continue;
        if (c == L'\\' || c == L'/' || c == L':')
            return npos;
        if (c == L'.')
            return p - 1;
    }

    return npos;
}


bool hasExtension(const wstring& s, const wstring& e)
{
    D1_ASSERT(!e.empty());
    D1_ASSERT(*begin(e) != L'.');

    if (s.size() < (1 + e.size()))
        return false;

    auto si = end(s) - e.size() - 1;

    if (*si++ != L'.')
        return false;

    const std::locale loc;
    for (auto ei = begin(e); ei != end(e); ++ei, ++si)
    {
        if (std::tolower(*si, loc) != std::tolower(*ei, loc))
            return false;
    }

    return true;
}


wstring RemoveExtension(const wstring& path, bool* removed_res)
{
    auto res = path;
    bool removed = false;

    auto pos = FindExtension(res);

    if (pos != npos)
    {
        res.erase(begin(res) + pos, end(res));
        removed = true;
    }

    if (removed_res)
        *removed_res = removed;

    return res;
}


wstring RemoveFileSpec(const wstring& path, bool* removed_res)
{
    auto res = path;
    bool removed = false;

    auto p = res.find_last_of(L"\\/");

    if (p == npos)
        goto exit;

    if (p + 1 == res.size())
        goto exit;

    res.erase(begin(res) + p, end(res));
    removed = true;

exit:
    if (removed_res)
        *removed_res = removed;

    return res;
}


// Removes the path portion of a fully qualified path and file.
wstring RemovePath(const wstring& path, bool* removed_res)
{
    auto res = path;
    bool removed = false;

    auto p = res.find_last_of(L"\\/");

    if (p == npos)
        goto exit;

    if (p + 1 == res.size())
        goto exit;

    res.erase(begin(res), begin(res) + p);
    removed = true;

exit:
    if (removed_res)
        *removed_res = removed;

    return res;
}

}

}

