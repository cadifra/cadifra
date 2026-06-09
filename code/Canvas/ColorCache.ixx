/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <gdiplus.h>

#include "d1/d1assert.h"

export module Canvas.ColorCache;

import Canvas.Color;

import d1.Observer;


namespace Canvas
{

export class ColorCache
{
    class Impl;
    static Impl& instance();

public:
    class Color // has value semantic, no default constructor
    {
    public:
        COLORREF getCOLORREF() const;
        bool operator==(const Color&) const = default;

    private:
        friend ColorCache;
        Color(const Canvas::Color& c):
            color_{ c }
        {
        }
        Canvas::Color color_;
    };

    Color getColor(const Canvas::Color& c) { return c; }

    class IObserver;
    auto getConnector() -> d1::Observer::C<IObserver>;
};


class ColorCache::IObserver
{
public:
    virtual void systemColorsChanged() = 0;

protected:
    ~IObserver() = default;
};

}


module : private;

import WinUtil.ListenerWindow;
import WinUtil.Messages;

import std;


namespace Canvas
{

using C = ColorCache;


class C::Impl
{
    using This = Impl;
    using SystemColorTable = std::array<COLORREF, ::Canvas::Color::LAST + 1>;

    WinUtil::ListenerWindow listenerWindow_;
    WinUtil::ProcRegistrar procReg_;
    d1::Observer::List<C::IObserver> observerList_;
    SystemColorTable systemColorTable_;

    void onSysColorChange(WinUtil::WM_SYSCOLORCHANGE_Msg);
    void systemColorsChanged();

public:
    Impl();

    COLORREF getSystemColor(SystemColorTable::size_type index)
    {
        return systemColorTable_.at(index);
    }

    auto getConnector() -> d1::Observer::C<C::IObserver>
    {
        return observerList_.getConnector();
    }
};


COLORREF C::Color::getCOLORREF() const
{
    if (color_.isAutomatic())
        return instance().getSystemColor(color_.getAutomaticColor());

    return color_.getRGB();
}


C::Impl::Impl():
    procReg_(listenerWindow_.getDispatcher(), 0)
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onSysColorChange);

    systemColorsChanged();
}


void C::Impl::onSysColorChange(WinUtil::WM_SYSCOLORCHANGE_Msg)
{
    systemColorsChanged();

    observerList_.notify(
        [](C::IObserver* obs) {
            obs->systemColorsChanged();
        });
}


void C::Impl::systemColorsChanged()
{
    for (int index = 0; auto& c : systemColorTable_)
        c = ::GetSysColor(index++);
}


auto C::getConnector() -> d1::Observer::C<C::IObserver>
{
    return instance().getConnector();
}


auto C::instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}

}
