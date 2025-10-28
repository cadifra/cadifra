/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1assert.h"

module WinUtil.Window;


namespace WinUtil
{

namespace
{
using C = WindowPlacementHandler;
}


C::WindowPlacementHandler()
{
}


C::WindowPlacementHandler(HWND w)
{
    GetFrom(w);
}


C::WindowPlacementHandler(const WindowPlacement& wp):
    imp{ std::make_unique<WindowPlacement>(wp) }
{
}


C::WindowPlacementHandler(const C& rhs):
    imp{ rhs.imp.get() ? std::make_unique<WindowPlacement>(*rhs.imp) : 0 }
{
}


auto C::operator=(const C& rhs) -> C&
{
    if (this == &rhs)
        return *this;

    imp = rhs.imp.get() ? std::make_unique<WindowPlacement>(*rhs.imp) : 0;
    return *this;
}


C::~WindowPlacementHandler() = default;


void C::GetFrom(HWND w)
{
    imp.reset();
    auto t = std::make_unique<WindowPlacement>();

    if (!::GetWindowPlacement(w, t.get()))
        return;

    imp = std::move(t);
}


void C::SetTo(HWND w) const
{
    if (!ok())
        return;

    const BOOL res = ::SetWindowPlacement(w, imp.get());
    D1_ASSERT(res);
}


void C::Read(const Registry::Key& k, const std::wstring& vn)
{
    imp.reset();
    auto t = std::make_unique<WindowPlacement>();

    using WP = WINDOWPLACEMENT;
    WP* wp = t.get();

    if (!Registry::Query(k, vn, reinterpret_cast<BYTE*>(wp), sizeof(WP)))
        return;

    if (t->length != sizeof(WP))
        return;

    imp = std::move(t);
}


void C::Write(const Registry::Key& k, const std::wstring& vn) const
{
    if (!ok())
        return;

    using WP = WINDOWPLACEMENT;
    WP* wp = imp.get();

    Registry::Set(k, vn, reinterpret_cast<const BYTE*>(wp), sizeof(WP));
}


void C::ChangeShowCmd(UINT showCmd)
{
    if (!ok())
        return;
    imp->showCmd = showCmd;
}

}

