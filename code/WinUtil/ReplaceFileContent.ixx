/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.ReplaceFileContent;

import d1.wintypes;

namespace WinUtil
{

export d1::DWORD ReplaceFileContent(d1::HANDLE source, d1::HANDLE target);

// Replaces the content of the file "target" with the content of
// the file "source". Returns 0 if successful or the system error
// code otherwise.

}
