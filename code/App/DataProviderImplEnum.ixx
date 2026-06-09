/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "WinUtil/d1Trace.h"

#include <Windows.h>

export module App.DataProviderImplEnum;

import App.IFormatProvider;

import d1.AutoComPtr;
import d1.RefCounter;

import WinUtil.GIClasses;

import std;


namespace App
{

export class DataProviderImplEnum: public ::IEnumFORMATETC
{
    using FP = std::vector<d1::AutoComPtr<App::IFormatProvider>>;

public:
    DataProviderImplEnum(
        FP::const_iterator start,
        FP::const_iterator stop,
        FP::const_iterator current,
        int currentIndex);

    //-- IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in]          */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return refCounter_.AddRef();
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        return refCounter_.Release(this);
    }


    //-- IEnumFORMATETC

    /* [local] */ HRESULT STDMETHODCALLTYPE Next(
        /* [in]                      */ ULONG celt,
        /* [length_is][size_is][out] */ FORMATETC* rgelt,
        /* [out]                     */ ULONG* pceltFetched) override;

    HRESULT STDMETHODCALLTYPE Skip(
        /* [in] */ ULONG celt) override;

    HRESULT STDMETHODCALLTYPE Reset() override;

    HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ IEnumFORMATETC** ppenum) override;

private:
    WinUtil::GIEnumFORMATETC<> GIEnumFORMATETC_ = *this;
    d1::RefCounter<DataProviderImplEnum> refCounter_;

    const FP::const_iterator start_, stop_;
    FP::const_iterator current_;
    int currentIndex_;
};

}

module : private;

import WinUtil.Debug;
import WinUtil.WinOstream;


D1_TRACE_DEFINE_FLAG(OLE, App::DataProvider)


namespace App
{

namespace
{
using C = DataProviderImplEnum;
}


C::DataProviderImplEnum(
    FP::const_iterator start,
    FP::const_iterator stop,
    FP::const_iterator current,
    int currentIndex):

    start_{ start },
    stop_{ stop },
    current_{ current },
    currentIndex_{ currentIndex }
{
}


HRESULT C::QueryInterface(REFIID riid, void** ppvObject)
{
    D1_TRACE("C::QueryInterface(" << riid << ")");

    if (riid == IID_IUnknown)
        *ppvObject = &GIEnumFORMATETC_;
    else if (riid == IID_IEnumFORMATETC)
        *ppvObject = &GIEnumFORMATETC_;
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


HRESULT C::Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched)
{
    D1_TRACE("DataProviderImplEnum::Next(" << celt << ")");

    UINT count;
    FORMATETC* fetc;

    for (count = 0, fetc = rgelt;
        (count < celt) and (current_ != stop_);
        ++count, ++fetc)
    {
        (*current_)->GetFormat(currentIndex_, *fetc);

        ++currentIndex_;

        if (currentIndex_ >= (*current_)->GetFormatCount())
        {
            ++current_;
            currentIndex_ = 0;
        }
    }

    if (pceltFetched)
        *pceltFetched = count;

    if (count == celt)
        return S_OK;
    else
        return S_FALSE;
}


HRESULT C::Skip(ULONG celt)
{
    D1_TRACE("DataProviderImplEnum::skip(" << celt << ")");

    for (UINT i = 0; i < celt; ++i)
    {
        if (current_ == stop_)
            return S_FALSE;

        ++currentIndex_;

        if (currentIndex_ >= (*current_)->GetFormatCount())
        {
            ++current_;
            currentIndex_ = 0;
        }
    }

    return S_OK;
}


HRESULT C::Reset()
{
    D1_TRACE("DataProviderImplEnum::Reset");

    current_ = start_;
    currentIndex_ = 0;
    return S_OK;
}


HRESULT C::Clone(IEnumFORMATETC** ppenum)
{
    D1_TRACE("DataProviderImplEnum::Clone");

    *ppenum = new DataProviderImplEnum(start_, stop_, current_, currentIndex_);
    (*ppenum)->AddRef();

    return S_OK;
}

}
