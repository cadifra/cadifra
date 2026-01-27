/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.SetCharsetInfo;


export namespace WinUtil
{

bool fromCharset(CHARSETINFO&, DWORD charset);

bool fromCodepage(CHARSETINFO&, DWORD codepage);

bool fromLCID(CHARSETINFO&, LCID lcid);

bool fromLANGID(CHARSETINFO&, LANGID langid);

}
