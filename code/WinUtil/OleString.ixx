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
    LPOLESTR OleStr_;

public:
    OleString(LPOLESTR s = 0):
        OleStr_{ s }
    {
    }

    ~OleString()
    {
        ::CoTaskMemFree(OleStr_);
    }

    operator LPOLESTR() const { return OleStr_; }

    LPOLESTR get() const { return OleStr_; }

    LPOLESTR* operator&()
    {
        D1_ASSERT(OleStr_ == 0);
        return &OleStr_;
    }

    OleString(const OleString&) = delete;
    OleString& operator=(const OleString&) = delete;
};

}
