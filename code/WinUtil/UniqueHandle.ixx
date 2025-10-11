/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil:UniqueHandle;

import std;


export namespace WinUtil
{

struct CloseHandleOp
{
    using pointer = HANDLE;

    void operator()(HANDLE h)
    {
        D1_VERIFY(::CloseHandle(h));
    }
};

using UniqueHandle = std::unique_ptr<HANDLE, CloseHandleOp>;


struct DestroyMenuOp
{
    using pointer = HMENU;

    void operator()(HMENU m)
    {
        D1_VERIFY(::DestroyMenu(m));
    }
};

using UniqueMenuHandle = std::unique_ptr<HMENU, DestroyMenuOp>;


export class FileHandleWrapper
{
    HANDLE itsH = INVALID_HANDLE_VALUE;

public:
    FileHandleWrapper(HANDLE h):
        itsH{ h } {}

    FileHandleWrapper() {}

    FileHandleWrapper(std::nullptr_t) {}

    explicit operator bool() const { return itsH != INVALID_HANDLE_VALUE; }

    bool operator==(std::nullptr_t) const { return itsH == INVALID_HANDLE_VALUE; }

    operator HANDLE() const { return itsH; }

    struct Del
    {
        using pointer = FileHandleWrapper;
        void operator()(FileHandleWrapper fhw) const;
    };
};


export using UniqueFileHandle =
    std::unique_ptr<FileHandleWrapper, FileHandleWrapper::Del>;

}
