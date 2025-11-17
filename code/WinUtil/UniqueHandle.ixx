/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.UniqueHandle;

import d1.wintypes;

import std;


export namespace WinUtil
{

struct CloseHandleOp
{
    using pointer = d1::HANDLE;

    void operator()(d1::HANDLE h);
};

using UniqueHandle = std::unique_ptr<d1::HANDLE, CloseHandleOp>;


struct DestroyMenuOp
{
    using pointer = d1::HMENU;

    void operator()(d1::HMENU m);
};

using UniqueMenuHandle = std::unique_ptr<d1::HMENU, DestroyMenuOp>;


export class FileHandleWrapper
{
    d1::HANDLE itsH = d1::invalid_handle_value;

public:
    FileHandleWrapper(d1::HANDLE h):
        itsH{ h } {}

    FileHandleWrapper() {}

    FileHandleWrapper(std::nullptr_t) {}

    explicit operator bool() const { return itsH != d1::invalid_handle_value; }

    bool operator==(std::nullptr_t) const { return itsH == d1::invalid_handle_value; }

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
