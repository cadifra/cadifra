/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Path;

import std;


namespace d1
{

export namespace Path
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

}
