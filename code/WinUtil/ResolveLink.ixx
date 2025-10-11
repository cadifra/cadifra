/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:ResolveLink;

import d1.wintypes;

import std;


namespace WinUtil
{

export std::wstring ResolveLink(const std::wstring& path, d1::HWND, bool fast = false);
/*
Returns path if path is not a shell-shortcut or returns
the resolved path.
Updates an outdated shell-shortcut if possible (shows
a dialog box, if needed).
Returns an empty string if the user cancels the update.
*/

}
