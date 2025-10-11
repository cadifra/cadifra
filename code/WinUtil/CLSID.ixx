/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:CLSID;

import d1.wintypes;


export namespace WinUtil
{

d1::CLSID ConvertToCLSID(const wchar_t* s);

}
