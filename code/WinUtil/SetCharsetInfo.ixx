/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.SetCharsetInfo;


export namespace WinUtil
{

bool FromCharset(CHARSETINFO&, DWORD charset);

bool FromCodepage(CHARSETINFO&, DWORD codepage);

bool FromLCID(CHARSETINFO&, LCID lcid);

bool FromLANGID(CHARSETINFO&, LANGID langid);

}
