/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

export module d1.Path;

import std;


export namespace d1::Path
{

using std::wstring;
using size_type = wstring::size_type;

constexpr size_type npos = std::wstring::npos;

size_type findExtension(const wstring& path);
// Returns the Position of the . preceding the extension of the
// filename in path or npos otherwise.

bool hasExtension(const std::wstring& s, const std::wstring& e);

wstring removeExtension(const wstring& path, bool* removed_something = 0);
// Removes extension part.

wstring removeFileSpec(const wstring& path, bool* removed_something = 0);
// Removes the trailing file name and {back}slash (if path has them).

wstring removePath(const wstring& path, bool* removed_something = 0);
// Removes the path portion of a fully qualified path and file.

};


module : private;


namespace d1::Path
{

size_type findExtension(const wstring& path)
{
    for (auto p = path.size(); p > 0; --p)
    {
        auto c = path[p - 1];
        if (c == L'\0')
            continue;
        if (c == L'\\' or c == L'/' or c == L':')
            return npos;
        if (c == L'.')
            return p - 1;
    }

    return npos;
}


bool hasExtension(const wstring& s, const wstring& e)
{
    D1_ASSERT(not e.empty());
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


wstring removeExtension(const wstring& path, bool* removed_res)
{
    auto res = path;
    bool removed = false;

    auto pos = findExtension(res);

    if (pos != npos)
    {
        res.erase(begin(res) + pos, end(res));
        removed = true;
    }

    if (removed_res)
        *removed_res = removed;

    return res;
}


wstring removeFileSpec(const wstring& path, bool* removed_res)
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
wstring removePath(const wstring& path, bool* removed_res)
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
