/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.ObjectDescriptor;

import App.IDataHolder;
import App.IFormatProvider;
import App.DataProvider;

import d1.Point;
import d1.Size;

import std;


namespace App
{

export struct ObjectDescriptor
{
    CLSID clsid = CLSID_NULL;
    std::wstring fullUserTypeName;
    std::wstring sourceOfCopy;
    d1::Size size = {};
    d1::Point dragPoint = {};
    DWORD status = 0;
    DWORD drawAspect = 0;

    ObjectDescriptor(
        const CLSID& clsid,
        const std::wstring& sourceOfCopy,
        const d1::Size& size = d1::Size{},
        const d1::Point& dragPoint = d1::Point{},
        DWORD drawAspect = DVASPECT_CONTENT);
    // fullUserTypeName and drawAspect are set according to the
    // values found in the registry for 'clsid'.

    ObjectDescriptor(
        const CLSID& clsid,
        const std::wstring& fullUserTypeName,
        const std::wstring& sourceOfCopy,
        const d1::Size& size,
        const d1::Point& dragPoint,
        DWORD status,
        DWORD drawAspect);

    ObjectDescriptor() {}
    ObjectDescriptor(const ObjectDescriptor&) = default;
    ObjectDescriptor& operator=(const ObjectDescriptor&) = default;
};


export namespace ObjectDescriptorFormatHandler
{
void addFormatProvider(
    DataProvider& dp, const ObjectDescriptor& od);
// Adds a new FormatProvider that contains the ObjectDescriptor od
// into the DataProvider object dp.

bool QueryExtract(IDataHolder& dh);
// Returns true if dh contains an ObjectDescriptor.

bool extract(IDataHolder& dh, ObjectDescriptor& od);
// If dh contains an ObjectDescriptor, the function extracts it and
// returns true. Otherwise it returns false and doesn't change od.
}


ObjectDescriptor::ObjectDescriptor(
    const CLSID& clsid,
    const std::wstring& sourceOfCopy,
    const d1::Size& size,
    const d1::Point& dragPoint,
    DWORD drawAspect):

    clsid{ clsid },
    sourceOfCopy{ sourceOfCopy },
    size{ size },
    dragPoint{ dragPoint },
    drawAspect{ drawAspect }
{
    if (S_OK != ::OleRegGetMiscStatus(
                    clsid,
                    DVASPECT_CONTENT,
                    &status))
    {
        status = 0;
    }

    LPOLESTR userType = 0;

    if (S_OK == ::OleRegGetUserType(
                    clsid,
                    USERCLASSTYPE_FULL,
                    &userType))
    {
        fullUserTypeName = userType;
        ::CoTaskMemFree(userType);
    }
}


ObjectDescriptor::ObjectDescriptor(
    const CLSID& clsid,
    const std::wstring& fullUserTypeName,
    const std::wstring& sourceOfCopy,
    const d1::Size& size,
    const d1::Point& dragPoint,
    DWORD status,
    DWORD drawAspect):

    clsid{ clsid },
    fullUserTypeName{ fullUserTypeName },
    sourceOfCopy{ sourceOfCopy },
    size{ size },
    dragPoint{ dragPoint },
    status{ status },
    drawAspect{ drawAspect }
{
}

}


module : private;

import WinUtil.ClipboardFormats;
import WinUtil.CreateObjectDescriptor;
import WinUtil.Global;
import WinUtil.OwnerSTGMEDIUM;


namespace App
{

enum
{
    IndexObjectDescriptor,
    FormatCount
};

const FORMATETC Formats[] = {
    { (CLIPFORMAT)WinUtil::ClipboardFormats::ObjectDescriptor(),
        0,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL },
};


class FormatProvider: public App::IFormatProvider
{
    App::ObjectDescriptor objectDescriptor_;

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
    FormatProvider(const App::ObjectDescriptor& od):
        objectDescriptor_{ od }
    {
    }
};


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
    case IndexObjectDescriptor:
        pmedium->tymed = TYMED_HGLOBAL;
        pmedium->pUnkForRelease = 0;
        pmedium->hGlobal = WinUtil::createObjectDescriptor(
            objectDescriptor_.clsid,
            objectDescriptor_.drawAspect,
            objectDescriptor_.size,
            objectDescriptor_.dragPoint,
            objectDescriptor_.status,
            objectDescriptor_.fullUserTypeName,
            objectDescriptor_.sourceOfCopy);
        if (not pmedium->hGlobal)
        {
            pmedium->tymed = TYMED_NULL;
            return E_OUTOFMEMORY;
        }
        else
            return S_OK;

    default:
        return E_NOTIMPL;
    }
}


class FormatTester: public App::IFormatTester
{
    bool QueryExtract(IDataObject* d) final;
};


bool FormatTester::QueryExtract(IDataObject* d)
{
    FORMATETC f = Formats[IndexObjectDescriptor];
    return (d->QueryGetData(&f) == S_OK) ? true : false;
}


class FormatExtractor: public App::IFormatExtractor
{
    App::ObjectDescriptor& objectDescriptor_;
    virtual bool extract(IDataObject*);

public:
    FormatExtractor(App::ObjectDescriptor& od):
        objectDescriptor_{ od }
    {
    }
};


bool FormatExtractor::extract(IDataObject* d)
{
    FORMATETC f = Formats[IndexObjectDescriptor];
    WinUtil::OwnerSTGMEDIUM stgm;

    if (S_OK != d->GetData(&f, &stgm))
        return false;

    auto od = WinUtil::GlobalLocker<::OBJECTDESCRIPTOR>{ stgm.hGlobal };

    if (not od)
        return false;

    objectDescriptor_.clsid = od->clsid;
    objectDescriptor_.size = { od->sizel.cx, od->sizel.cy };
    objectDescriptor_.dragPoint = { od->pointl.x, od->pointl.y };
    objectDescriptor_.status = od->dwStatus;
    objectDescriptor_.drawAspect = od->dwDrawAspect;

    if (od->dwFullUserTypeName)
    {
        objectDescriptor_.fullUserTypeName = reinterpret_cast<wchar_t*>(
            reinterpret_cast<BYTE*>(od.getPtr()) + od->dwFullUserTypeName);
    }
    else
    {
        objectDescriptor_.fullUserTypeName.erase();
    }

    if (od->dwSrcOfCopy)
    {
        objectDescriptor_.sourceOfCopy = reinterpret_cast<wchar_t*>(
            reinterpret_cast<BYTE*>(od.getPtr()) + od->dwSrcOfCopy);
    }
    else
    {
        objectDescriptor_.sourceOfCopy.erase();
    }

    return true;
}


namespace ObjectDescriptorFormatHandler
{

void addFormatProvider(
    DataProvider& dp, const ObjectDescriptor& od)
{
    dp.addFormat(*new FormatProvider(od));
}


bool QueryExtract(IDataHolder& dh)
{
    FormatTester t;
    return dh.QueryExtract(t);
}


bool extract(IDataHolder& dh, ObjectDescriptor& od)
{
    auto f = FormatExtractor{ od };
    return dh.extract(f);
}

}

}
