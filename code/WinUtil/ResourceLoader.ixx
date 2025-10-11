/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:ResourceLoader;

import :Gdi;
import :MenuHandle;

import std;


namespace WinUtil
{

export class ResourceLoader
{
public:
    static ResourceLoader& Instance();

    virtual void Init(HINSTANCE) = 0;
    // Init has to be called exactly once before any other
    // function below.

    virtual std::wstring GetString(UINT id) const = 0;
    // returns the string 'id' from the resources

    using StringListType = std::vector<std::wstring>;
    virtual std::wstring GetFormatString(
        UINT id, const StringListType& list) const = 0;
    // gets the string 'id' from the resources and replaces the %1..%n
    // with the strings in 'list'
    // ## WARNING: If the format string specified by id contains placeholders
    // %1..%n, but there are less than n actual entries in list, the behaviour
    // of GetFormatString() is not defined (i.e. may crash).

    virtual HICON GetIcon(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual HACCEL GetAccelerators(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual RootMenuHandle GetMenu(WORD id) const = 0;

    virtual HCURSOR GetCursor(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual GdiObjectOwner<HBITMAP> GetBitmap(WORD id) const = 0;


    virtual HINSTANCE GetInstanceHandle() const = 0;

protected:
    ~ResourceLoader() = default;
};


}
