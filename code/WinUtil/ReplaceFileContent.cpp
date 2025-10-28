/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.ReplaceFileContent;

import WinUtil.UniqueHandle;


namespace WinUtil
{

DWORD ReplaceFileContent(HANDLE source, HANDLE target)
{
    const DWORD size = GetFileSize(source, 0);

    if (size == INVALID_FILE_SIZE)
        return GetLastError();

    auto sm = UniqueHandle{ CreateFileMapping(
        source,
        0,             // lpAttributes
        PAGE_READONLY, // flProtect
        0,             // dwMaximumSizeHigh
        0,             // dwMaximumSizeLow
        0              // lpName
        ) };

    if (!sm)
        return GetLastError();

    LPCVOID v = MapViewOfFile(
        sm.get(),
        FILE_MAP_READ, // dwDesiredAccess
        0,             // dwFileOffsetHigh
        0,             // dwFileOffsetLow
        0              // dwNumberOfBytesToMap
    );

    if (!v)
        return GetLastError();


    class ViewUnmapper
    {
        LPCVOID itsView;

    public:
        ViewUnmapper(LPCVOID v):
            itsView{ v } {}
        ~ViewUnmapper() { UnmapViewOfFile(itsView); }
    };

    auto vu = ViewUnmapper{ v };


    const DWORD target_size = GetFileSize(target, 0);

    if (target_size == INVALID_FILE_SIZE)
        return GetLastError();

    if (target_size < size)
    {
        LARGE_INTEGER p;
        p.QuadPart = size;
        // Size is an unsigned parameter and SetFilePointer uses signed parameters
        // -> size may be too big to fit in an 32bit signed integer.

        DWORD r = SetFilePointer(target, p.LowPart, &p.HighPart, FILE_BEGIN);

        if (r == INVALID_SET_FILE_POINTER)
        {
            DWORD err = GetLastError();
            if (err != NO_ERROR)
                return err;
        }

        BOOL res = SetEndOfFile(target);

        if (!res)
            return GetLastError();
    }

    DWORD r = SetFilePointer(target, 0, 0, FILE_BEGIN);

    if (r == INVALID_SET_FILE_POINTER)
        return GetLastError();

    DWORD b = 0;

    BOOL res = WriteFile(
        target,
        v,    // lpBuffer
        size, // nNumberOfBytesToWrite
        &b,   // lpNumberOfBytesWritten
        0     // lpOverlapped
    );

    if (!res)
        return GetLastError();

    if (b != size)
        return ERROR_INTERNAL_ERROR;

    if (target_size > size)
    {
        res = SetEndOfFile(target);

        if (!res)
            return GetLastError();
    }

    res = FlushFileBuffers(target);

    if (!res)
        return GetLastError();

    return ERROR_SUCCESS;
}


}
