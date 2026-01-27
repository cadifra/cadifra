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
    getFrom(w);
}


C::WindowPlacementHandler(const WindowPlacement& wp):
    imp_{ std::make_unique<WindowPlacement>(wp) }
{
}


C::WindowPlacementHandler(const C& rhs):
    imp_{ rhs.imp_.get() ? std::make_unique<WindowPlacement>(*rhs.imp_) : 0 }
{
}


auto C::operator=(const C& rhs) -> C&
{
    if (this == &rhs)
        return *this;

    imp_ = rhs.imp_.get() ? std::make_unique<WindowPlacement>(*rhs.imp_) : 0;
    return *this;
}


C::~WindowPlacementHandler() = default;


void C::getFrom(HWND w)
{
    imp_.reset();
    auto t = std::make_unique<WindowPlacement>();

    if (not ::GetWindowPlacement(w, t.get()))
        return;

    imp_ = std::move(t);
}


void C::setTo(HWND w) const
{
    if (not ok())
        return;

    const BOOL res = ::SetWindowPlacement(w, imp_.get());
    D1_ASSERT(res);
}


void C::read(const Registry::Key& k, const std::wstring& vn)
{
    imp_.reset();
    auto t = std::make_unique<WindowPlacement>();

    using WP = WINDOWPLACEMENT;
    WP* wp = t.get();

    if (not Registry::query(k, vn, reinterpret_cast<BYTE*>(wp), sizeof(WP)))
        return;

    if (t->length != sizeof(WP))
        return;

    imp_ = std::move(t);
}


void C::write(const Registry::Key& k, const std::wstring& vn) const
{
    if (not ok())
        return;

    using WP = WINDOWPLACEMENT;
    WP* wp = imp_.get();

    Registry::set(k, vn, reinterpret_cast<const BYTE*>(wp), sizeof(WP));
}


void C::changeShowCmd(UINT showCmd)
{
    if (not ok())
        return;
    imp_->showCmd = showCmd;
}

}

