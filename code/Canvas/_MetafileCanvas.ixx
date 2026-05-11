/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.MetafileCanvas;

import Canvas.Canvas;

import WinUtil.types;


namespace Canvas
{

export class IMetafileCanvas: public virtual Canvas
{
public:
    virtual WinUtil::STGMEDIUM createEnhancedMetafile(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createWindowsMetafile(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createBitmap(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createPNG(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual void createPNG(
        const wchar_t* filename, d1::uint32 borderSize /*himetric units*/) const = 0;
};

}
