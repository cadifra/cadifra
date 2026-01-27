/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.ResourceLoader;

import WinUtil.Gdi;
import WinUtil.MenuHandle;

import std;


namespace WinUtil
{

export class ResourceLoader
{
public:
    static ResourceLoader& instance();

    virtual void init(HINSTANCE) = 0;
    // Init has to be called exactly once before any other
    // function below.

    virtual std::wstring getString(UINT id) const = 0;
    // returns the string 'id' from the resources

    using StringListType = std::vector<std::wstring>;
    virtual std::wstring getFormatString(
        UINT id, const StringListType& list) const = 0;
    // gets the string 'id' from the resources and replaces the %1..%n
    // with the strings in 'list'
    // ## WARNING: If the format string specified by id contains placeholders
    // %1..%n, but there are less than n actual entries in list, the behaviour
    // of getFormatString() is not defined (i.e. may crash).

    virtual HICON getIcon(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual HACCEL getAccelerators(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual RootMenuHandle getMenu(WORD id) const = 0;

    virtual HCURSOR getCursor(WORD id) const = 0;
    // The returned handle refers to a shared resource. The caller
    // does not need to destroy it after use.

    virtual auto getBitmap(WORD id) const -> GdiObjectOwner<HBITMAP> = 0;

    virtual HINSTANCE getInstanceHandle() const = 0;

protected:
    ~ResourceLoader() = default;
};


}
