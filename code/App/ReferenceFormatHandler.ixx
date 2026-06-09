/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.ReferenceFormatHandler;

import App.DataProvider;
import App.IDataHolder;


namespace App
{

export class IDataObjectReferenced /* abstract */
//
//  An Object which is referenced from a windows ::IDataObject.
//
//  An ::IDataObject is used to carry drag and drop or clipboard data (defined
//  by windows API).
//
//  Use IDataObjectReferenced to embed a pointer to an IDataObjectReferenced
//  in an ::IDataObject.
//
//  Pointers to IDataObjectReferenced can be inserted into ::IDataObject
//  by using the function App::ReferenceFormatHandler::addFormatProvider.
//
//  This module contains functions to test, whether an ::IDataObject
//  contains an IDataObjectReferenced pointer (queryExtract) and to extract
//  an IDataObjectReferenced pointer from ::IDataObject (extract).
//
//  The functions in ReferenceFormatHandler ensure, that an IDataObjectReferenced
//  pointer can only be extracted if it was inserted into the ::IDataObject
//  by the same application instance.
//
//  Warning: Make sure, that the concrete object which the IDataObjectReferenced
//  pointer points to is still alive when using the extracted pointer.
//
//  Use IDataObjectReferenced as a base class for concrete derivations.
//
{
public:
    virtual ~IDataObjectReferenced() = 0 {};
};


export namespace ReferenceFormatHandler
//
//  Functions to insert and extract IDataObjectReferenced pointers
//  into/from windows ::IDataObjects (clipboard and drag/drop content).
//
//  The ::IDataObject are hidden behind the DataProvider and the
//  IDataHolder parameter.
//
{
void addFormatProvider(DataProvider& dp, IDataObjectReferenced& p);
// Adds a new FormatProvider that contains the pointer p into the
// DataProvider dp.
// Use this function to insert a pointer to an IDataObjectReferenced
// into a windows ::IDataObject (clipboard or drag and drop content).

bool queryExtract(IDataHolder& dh);
// If dh contains a pointer to an IDataObjectReferenced residing in
// the process of the same application instance as the calling thread,
// queryExtract returns true. Otherwise, queryExtract returns false.
// queryExtract guaranties that IDataObjectReferenced pointers can only be
// transferred via a windows ::IDataObject if the provider and the
// consumer reside in the same process.

auto extract(IDataHolder& dh) -> IDataObjectReferenced*;
// If dh contains a pointer to an IDataObjectReferenced of the same
// application instance, extract returns 'x'. Otherwise extract returns 0.
}

}

module : private;

import App.IFormatProvider;

import WinUtil.PrivateClipFormat;
import WinUtil.Global;
import WinUtil.OwnerSTGMEDIUM;


namespace App
{

namespace
{

enum
{
    IndexDataObjectReferenced,
    FormatCount
};

const FORMATETC Formats[] = {
    { WinUtil::PrivateClipFormat::instance().getCLIPFORMAT(),
        0,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL }
};


class FormatProvider: public IFormatProvider
{
    IDataObjectReferenced& referenced_;

    int GetFormatCount() const final { return FormatCount; }

    void GetFormat(int formatIndex, tagFORMATETC&) const final;

    HRESULT GetData(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final;

    HRESULT GetDataHere(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final { return E_NOTIMPL; }

public:
    FormatProvider(App::IDataObjectReferenced&);
};


FormatProvider::FormatProvider(App::IDataObjectReferenced& r):
    referenced_{ r }
{
}


void FormatProvider::GetFormat(int formatIndex, tagFORMATETC& etc) const
{
    D1_ASSERT(0 <= formatIndex);
    D1_ASSERT(formatIndex < FormatCount);
    etc = Formats[formatIndex];
}


HRESULT FormatProvider::GetData(
    int formatIndex,
    tagFORMATETC* pFormatetc,
    tagSTGMEDIUM* pmedium)
{
    switch (formatIndex)
    {
    case IndexDataObjectReferenced:
    {
        auto g = WinUtil::GlobalOwner{ ::GlobalAlloc(GHND, sizeof(IDataObjectReferenced*)) };
        if (not g.get())
            return E_UNEXPECTED;

        auto p = WinUtil::GlobalLocker<IDataObjectReferenced*>{ g.get() };
        *p = &referenced_;

        pmedium->tymed = TYMED_HGLOBAL;
        pmedium->hGlobal = g.release();
        pmedium->pUnkForRelease = 0;
        return S_OK;
    }

    default:
        return E_NOTIMPL;
    }
}


class FormatExtractor: public App::IFormatExtractor
{
    IDataObjectReferenced* referenced_ = nullptr;
    bool extract(IDataObject*) final;

public:
    FormatExtractor() {}

    auto GetPtr() const -> IDataObjectReferenced*
    {
        return referenced_;
    }
};


bool FormatExtractor::extract(IDataObject* d)
{
    if (not detectInProcess(d))
        return false;

    FORMATETC f = Formats[IndexDataObjectReferenced];

    WinUtil::OwnerSTGMEDIUM stgm;

    if (S_OK != d->GetData(&f, &stgm))
        return false;

    auto p = WinUtil::GlobalLocker<IDataObjectReferenced*>{ stgm.hGlobal };

    if (not p.getPtr())
        return false;

    referenced_ = *p;

    if (not referenced_)
        return false;

    return true;
}

}

namespace ReferenceFormatHandler
{

void addFormatProvider(DataProvider& dp, IDataObjectReferenced& p)
{
    dp.addFormat(*new FormatProvider(p));
}


bool queryExtract(IDataHolder& dh)
{
    FormatExtractor f;
    dh.extract(f);
    return f.GetPtr() ? true : false;
}


auto extract(IDataHolder& dh) -> IDataObjectReferenced*
{
    FormatExtractor f;
    dh.extract(f);
    return f.GetPtr();
}

}

}
