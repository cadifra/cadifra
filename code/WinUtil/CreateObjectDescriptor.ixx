/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.CreateObjectDescriptor;

import d1.Point;
import d1.Size;
import d1.wintypes;

import std;


namespace WinUtil
{

export d1::HGLOBAL CreateObjectDescriptor(
    d1::CLSID clsid,
    d1::DWORD dwAspect,
    const d1::Size size,
    const d1::Point upperLeftPoint,
    d1::DWORD miscStatus,
    const std::wstring& objectName,
    const std::wstring& objectSource);

// The caller is responsible to delete the allocated memory.

}
