/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.CreateObjectDescriptor;

import WinUtil.Global;


namespace WinUtil
{

HGLOBAL CreateObjectDescriptor(
    CLSID clsid,
    DWORD dwAspect,
    const d1::Size size,
    const d1::Point upperLeftPoint,
    DWORD miscStatus,
    const std::wstring& objectName,
    const std::wstring& objectSource)
{
    auto objectNameSize =
        objectName.size()
            ? (objectName.size() + 1) * sizeof(std::wstring::value_type)
            : 0;

    auto objectSourceSize =
        objectSource.size()
            ? (objectSource.size() + 1) * sizeof(std::wstring::value_type)
            : 0;

    auto memsize = sizeof(OBJECTDESCRIPTOR) +
                  objectNameSize + objectSourceSize;


    auto objectNameOffset = sizeof(OBJECTDESCRIPTOR);
    auto objectSourceOffset = objectNameOffset + objectNameSize;


    HGLOBAL glob = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, memsize);

    if (!glob)
        return 0;

    auto od = WinUtil::GlobalLocker<OBJECTDESCRIPTOR>{ glob };

    if (!od)
        return 0;

    od->cbSize = static_cast<ULONG>(memsize);
    od->clsid = clsid;
    od->dwDrawAspect = dwAspect;
    od->sizel.cx = size.w;
    od->sizel.cy = size.h;
    od->pointl.x = upperLeftPoint.x;
    od->pointl.y = upperLeftPoint.y;
    od->dwStatus = miscStatus;
    od->dwFullUserTypeName = static_cast<DWORD>(objectNameSize ? objectNameOffset : 0);
    od->dwSrcOfCopy = static_cast<DWORD>(objectSourceSize ? objectSourceOffset : 0);

    ::memcpy(
        reinterpret_cast<BYTE*>(od.GetPtr()) + objectNameOffset,
        objectName.c_str(),
        objectNameSize);

    ::memcpy(
        reinterpret_cast<BYTE*>(od.GetPtr()) + objectSourceOffset,
        objectSource.c_str(),
        objectSourceSize);

    return glob;
}

}
