/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Metafile;

import App.Com;

import Canvas.Base;

import d1.AutoComPtr;
import d1.Rect;

import WinUtil.types;

import Core.Main;

import std;


namespace Editor
{

export auto createMetafileProvider(Core::IDiagram&)
    -> d1::AutoComPtr<App::IFormatProvider>;
// the caller is responsible to delete the created object


class MetafileEvalString
{
    Canvas::Group custom_;

public:
    MetafileEvalString(Canvas::Canvas& c);
};


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
