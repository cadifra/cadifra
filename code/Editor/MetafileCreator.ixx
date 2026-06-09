/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include "Resources/StringMacro.h"

#include <Windows.h>

export module Editor.MetafileCreator;

import Canvas.Canvas;
import Canvas.Group;

import d1.AutoComPtr;
import d1.Rect;

import WinUtil.types;

import Core.Main;

import std;


namespace Editor
{

export class MetafileCreator
{
    class Cache;

    Core::IClub& club_;
    std::unique_ptr<Cache> cache_;

public:
    MetafileCreator(Core::IClub&);

    MetafileCreator(const MetafileCreator&) = delete;
    MetafileCreator& operator=(const MetafileCreator&) = delete;

    virtual ~MetafileCreator();

    d1::fnRect getLogicBounds();

    constexpr static int BorderSizeDefault = 200; /*himetric units*/

    auto createEnhancedMetafile(d1::uint32 borderSize = BorderSizeDefault) -> WinUtil::STGMEDIUM;
    auto createWindowsMetafile(d1::uint32 borderSize = BorderSizeDefault) -> WinUtil::STGMEDIUM;
    auto createBitmap(d1::uint32 borderSize = BorderSizeDefault) -> WinUtil::STGMEDIUM;
    auto createPNG(d1::uint32 borderSize = BorderSizeDefault) -> WinUtil::STGMEDIUM;
    void createPNG(const wchar_t* filename, d1::uint32 borderSize = BorderSizeDefault);

private:
    void createCache();
};

}


module : private;

import Editor.LicenseInfo;
import Editor.MetafileEvalString;

import Canvas.MetafileCanvas;

import Core.Main;

import StaticCanvas.MetafileCanvas;

import View.ReadOnlyDiagramView;

import Resources;


D1_RESOURCES_USE_STRING(AppName)


namespace Editor
{

constexpr d1::Point NullSize{ 4000, 4000 }; // in HIMETRIC units

using C = MetafileCreator;


class C::Cache
{
public:
    std::unique_ptr<Canvas::IMetafileCanvas> mc;
    std::unique_ptr<View::ReadOnlyDiagramView> rodv;
};


C::MetafileCreator(
    Core::IClub& c):
    club_{ c }
{
}


C::~MetafileCreator()
{
}


void C::createCache()
{
    if (cache_)
        return;

    cache_ = std::make_unique<Cache>();

    cache_->mc =
        StaticCanvas::MetafileCanvas::create(
            NullSize,
            LicenseInfo::instance().evaluationMode()
                ? Resources::EvaluationStrings::appName()
                : Resources::Strings::AppName,
            L"");

    cache_->rodv = std::make_unique<View::ReadOnlyDiagramView>(
        club_, *cache_->mc);

    auto a = [&](auto* me) { cache_->rodv->insert(*me); };
    std::ranges::for_each(club_, a);

    cache_->rodv->update();
}


auto C::getLogicBounds() -> d1::fnRect
{
    createCache();
    return cache_->mc->getLogicBounds();
}


auto C::createEnhancedMetafile(d1::uint32 borderSize) -> STGMEDIUM
{
    createCache();
    auto mes = MetafileEvalString{ *cache_->mc };
    return cache_->mc->createEnhancedMetafile(borderSize);
}


auto C::createWindowsMetafile(d1::uint32 borderSize) -> STGMEDIUM
{
    createCache();
    auto mes = MetafileEvalString{ *cache_->mc };
    return cache_->mc->createWindowsMetafile(borderSize);
}


auto C::createBitmap(d1::uint32 borderSize) -> STGMEDIUM
{
    createCache();
    auto mes = MetafileEvalString{ *cache_->mc };
    return cache_->mc->createBitmap(borderSize);
}


auto C::createPNG(d1::uint32 borderSize) -> STGMEDIUM
{
    createCache();
    auto mes = MetafileEvalString{ *cache_->mc };
    return cache_->mc->createPNG(borderSize);
}


void C::createPNG(const wchar_t* filename, d1::uint32 borderSize)
{
    createCache();
    auto mes = MetafileEvalString{ *cache_->mc };
    return cache_->mc->createPNG(filename, borderSize);
}

}
