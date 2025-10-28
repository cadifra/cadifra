/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.OleString;


namespace WinUtil
{

export class OleString
{
    LPOLESTR itsOleStr;

public:
    OleString(LPOLESTR s = 0):
        itsOleStr{ s }
    {
    }

    ~OleString()
    {
        ::CoTaskMemFree(itsOleStr);
    }

    operator LPOLESTR() const { return itsOleStr; }

    LPOLESTR get() const { return itsOleStr; }

    LPOLESTR* operator&()
    {
        D1_ASSERT(itsOleStr == 0);
        return &itsOleStr;
    }

    OleString(const OleString&) = delete;
    OleString& operator=(const OleString&) = delete;
};

}
