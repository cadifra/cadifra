/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.MetafileProvider;

import App.IFormatProvider;

import Core.Main;

import d1.AutoComPtr;


namespace Editor
{

export auto createMetafileProvider(Core::IDiagram&)
    -> d1::AutoComPtr<App::IFormatProvider>;
// the caller is responsible to delete the created object

}

module : private;

import Editor.MetafileEvalString;

import d1.Point;

import Canvas.MetafileCanvas;

import StaticCanvas.MetafileCanvas;

import View.ReadOnlyDiagramView;

import WinUtil.ClipboardFormats;

import std;


namespace Editor
{

constexpr d1::Point NullSize = { 4000, 4000 }; // in HIMETRIC units
constexpr d1::uint32 BorderSize = 200;      // in HIMETRIC units

// MetafileProvider provides 2 formats:
constexpr int IndexEnhMetafile = 0;
constexpr int IndexMetafile = 1;
constexpr int IndexPNG = 2;
// const int IndexBitmap      = 3;
constexpr int FormatCount = 3;


const FORMATETC& formats(int index)
{
    static const auto formats = std::vector<FORMATETC>{
        { CF_ENHMETAFILE,
            0,
            DVASPECT_CONTENT,
            -1,
            TYMED_ENHMF },
        { CF_METAFILEPICT,
            0,
            DVASPECT_CONTENT,
            -1,
            TYMED_MFPICT },
        { (CLIPFORMAT)WinUtil::ClipboardFormats::PNG(),
            0,
            DVASPECT_CONTENT,
            -1,
            TYMED_ISTREAM } /*,
                           {
                             CF_DIB,
                             0,
                             DVASPECT_CONTENT,
                             -1,
                             TYMED_HGLOBAL
                           }*/
    };
    return formats.at(index);
}


class MetafileProvider:
    public App::IFormatProvider
{
    std::unique_ptr<Canvas::IMetafileCanvas> metafileCanvas_;
    std::unique_ptr<View::ReadOnlyDiagramView> view_;
    Core::IDiagram& diagram_;

    virtual int GetFormatCount() const { return FormatCount; }

    virtual void GetFormat(int index, tagFORMATETC&) const;

    virtual HRESULT GetData(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium);

    virtual HRESULT GetDataHere(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) { return E_NOTIMPL; }

public:
    MetafileProvider(Core::IDiagram& d):
        metafileCanvas_{ StaticCanvas::MetafileCanvas::create(NullSize, L"", L"") },
        view_{ std::make_unique<View::ReadOnlyDiagramView>(d, *metafileCanvas_) },
        diagram_{ d }
    {
        diagram_.add(*view_);
    };

    virtual ~MetafileProvider()
    {
        diagram_.forget(*view_);
    }
};


void MetafileProvider::GetFormat(int index, tagFORMATETC& fetc) const
{
    D1_ASSERT(0 <= index);
    D1_ASSERT(index < FormatCount);
    fetc = formats(index);
}


HRESULT MetafileProvider::GetData(
    int index,
    tagFORMATETC* pFormatetc,
    tagSTGMEDIUM* pmedium)
{
    switch (index)
    {
    case IndexEnhMetafile:
    {
        view_->update();
        auto mes = Editor::MetafileEvalString{ *metafileCanvas_ };
        *pmedium = metafileCanvas_->createEnhancedMetafile(BorderSize);
        return S_OK;
    }

    case IndexMetafile:
    {
        view_->update();
        auto mes = Editor::MetafileEvalString{ *metafileCanvas_ };
        *pmedium = metafileCanvas_->createWindowsMetafile(BorderSize);
        return S_OK;
    }

    case IndexPNG:
    {
        view_->update();
        auto mes = Editor::MetafileEvalString{ *metafileCanvas_ };
        *pmedium = metafileCanvas_->createPNG(BorderSize);
        return S_OK;
    }
        /*
            case IndexBitmap:
            {
              view_->update();
              Editor::MetafileEvalString mes(*metafileCanvas_);
              *pmedium = metafileCanvas_->createBitmap(BorderSize);
              return S_OK;
            }
        */
    default:
        return E_NOTIMPL;
    }
}


auto createMetafileProvider(Core::IDiagram& d)
    -> d1::AutoComPtr<App::IFormatProvider>

{
    return new MetafileProvider(d);
}

}
