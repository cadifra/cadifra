/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.ConsumeDropHandle;


namespace WinUtil
{

ConsumeDropHandle::ConsumeDropHandle(HDROP hDrop)
{
    POINT p;
    if (::DragQueryPoint(hDrop, &p))
    {
        point.x = p.x;
        point.y = p.y;
    }

    UINT count = ::DragQueryFile(
        hDrop,
        -1, // call number of files
        0,  // lpszFile
        0   // cch
    );

    fileList.resize(count);

    auto pF = begin(fileList);

    for (UINT i = 0; i < count; ++i, ++pF)
    {
        int len = ::DragQueryFile(
            hDrop,
            i,
            0, // lpszFile
            0  // cch
        );

        if (len)
        {
            pF->resize(len + 1);

            len = ::DragQueryFile(
                hDrop,
                i,
                const_cast<wchar_t*>(pF->c_str()), // lpszFile
                static_cast<UINT>(pF->size())      // cch
            );

            pF->resize(len);
        }
    }

    ::DragFinish(hDrop);
}

}
