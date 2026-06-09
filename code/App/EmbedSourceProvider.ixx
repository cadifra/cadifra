/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.EmbedSourceProvider;

import App.IFormatProvider;

import d1.AutoComPtr;


namespace App
{

export auto createEmbedSourceProvider(::IPersistStorage*)
    -> d1::AutoComPtr<IFormatProvider>;
// The caller is responsible to delete the created object.
// The object will not AddRef the IPersistStorage interface
// ("reference only").

}

module : private;

import WinUtil.ClipboardFormats;
import WinUtil.ComException;
import WinUtil.GuardedFunctionCall;


namespace
{

class EmbedSourceProvider: public App::IFormatProvider
{
    friend class GetDataHereCall;

    ::IPersistStorage* persistStg_;

    //-- IFormatProvider

    void GetFormat(int index, tagFORMATETC&) const final;

    HRESULT GetData(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final;

    HRESULT GetDataHere(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final;

    //--

public:
    EmbedSourceProvider(::IPersistStorage* ps);
};


EmbedSourceProvider::EmbedSourceProvider(::IPersistStorage* ps):
    persistStg_{ ps }
{
}


void EmbedSourceProvider::GetFormat(int, tagFORMATETC& fetc) const
{
    fetc.cfFormat = WinUtil::ClipboardFormats::EmbedSource();
    fetc.ptd = 0;
    fetc.dwAspect = DVASPECT_CONTENT;
    fetc.lindex = -1;
    fetc.tymed = TYMED_ISTORAGE;
}


HRESULT EmbedSourceProvider::GetData(
    int, tagFORMATETC* pFormatetc, tagSTGMEDIUM* pmedium)
{
    return E_NOTIMPL;
}


using GFC_HRESULT = WinUtil::GuardedFunctionCallRet<HRESULT>;

class GetDataHereCall: public GFC_HRESULT
{
    EmbedSourceProvider& esp_;
    int index_;
    tagFORMATETC* pFormatetc_;
    tagSTGMEDIUM* pmedium_;

public:
    GetDataHereCall(
        EmbedSourceProvider& p1, int p2, tagFORMATETC* p3, tagSTGMEDIUM* p4):
        GFC_HRESULT{ "App::EmbedSourceProvider::GetDataHere", E_UNEXPECTED },
        esp_{ p1 }, index_{ p2 }, pFormatetc_{ p3 }, pmedium_{ p4 }
    {
    }
    HRESULT implementCallRet();
};

HRESULT EmbedSourceProvider::GetDataHere(
    int index, tagFORMATETC* pFormatetc, tagSTGMEDIUM* pmedium)
{
    auto c = GetDataHereCall{ *this, index, pFormatetc, pmedium };
    c.execute();
    return c.res();
}

HRESULT GetDataHereCall::implementCallRet()
{
    using WinUtil::ComException;

    try
    {
        if (not pmedium_ or not pFormatetc_)
            return E_POINTER;

        if (pmedium_->tymed != TYMED_ISTORAGE)
            return DV_E_TYMED;

        HRESULT res = esp_.persistStg_->Save(pmedium_->pstg, false);
        ComException::check(res);

        res = esp_.persistStg_->SaveCompleted(0);
        ComException::check(res);

        CLSID clsid;
        res = esp_.persistStg_->GetClassID(&clsid);
        ComException::check(res);

        res = ::WriteClassStg(pmedium_->pstg, clsid);
        ComException::check(res);

        return S_OK;
    }
    catch (ComException e)
    {
        return e.getHRESULT();
    }
}

}


namespace App
{

auto createEmbedSourceProvider(::IPersistStorage* ps)
    -> d1::AutoComPtr<IFormatProvider>
{
    return new EmbedSourceProvider(ps);
}

}
