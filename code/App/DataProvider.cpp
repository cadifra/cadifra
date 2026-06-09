/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "WinUtil/d1Trace.h"

#include "d1/d1assert.h"

#include <Windows.h>
#include <objidl.h>

module App.DataProvider;

import App.DataProviderImplEnum;

import d1.RefCounter;

import WinUtil.GIClasses;
import WinUtil.Debug;
import WinUtil.WinOstream;

import std;


D1_TRACE_DEFINE_FLAG(OLE, App::DataProvider)


namespace App
{

namespace
{

class __declspec(uuid("CB3EB24F-3736-4b4f-9B34-983B21A3A061"))
IDetectInProcess: public IUnknown
{
    // no methods
};

using FormatProviders =
    std::vector<d1::AutoComPtr<App::IFormatProvider>>;
}


class DataProvider::Impl:
    public IDataObject,
    public IDetectInProcess
{
public:
    virtual ~Impl() = default;

    void addFormat(IFormatProvider&);

    void SendOnStop();
    void SendOnDataChange();


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


    //-- IDataObject

    /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
        /* [unique][in] */ FORMATETC* pformatetcIn,
        /* [out]        */ STGMEDIUM* pmedium) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [out][in]    */ STGMEDIUM* pmedium) override;

    HRESULT STDMETHODCALLTYPE QueryGetData(
        /* [unique][in] */ FORMATETC* pformatetc) override;

    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        /* [unique][in] */ FORMATETC* pformatectIn,
        /* [out]        */ FORMATETC* pformatetcOut) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [unique][in] */ STGMEDIUM* pmedium,
        /* [in]         */ BOOL fRelease) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        /* [in]  */ DWORD dwDirection,
        /* [out] */ IEnumFORMATETC** ppenumFormatEtc) override;

    HRESULT STDMETHODCALLTYPE DAdvise(
        /* [in]         */ FORMATETC* pformatetc,
        /* [in]         */ DWORD advf,
        /* [unique][in] */ IAdviseSink* pAdvSink,
        /* [out]        */ DWORD* pdwConnection) override;

    HRESULT STDMETHODCALLTYPE DUnadvise(
        /* [in]  */ DWORD dwConnection) override;

    HRESULT STDMETHODCALLTYPE EnumDAdvise(
        /* [out] */ IEnumSTATDATA** ppenumAdvise) override;

private:
    WinUtil::GIDataObject<> GIDataObject_ = *this;
    d1::RefCounter<Impl> refCounter_;
    d1::AutoComPtr<IDataAdviseHolder> adviseHolder_;
    bool changed_ = false;

    FormatProviders formatProviders_;

    IDataAdviseHolder* GetAdviseHolder();
};


namespace
{
using C = DataProvider;
}


C::DataProvider():
    impl_{ new Impl }
{
}


C::~DataProvider()
{
}


C::DataProvider(const DataProvider& rhs):
    impl_{ rhs.impl_ }
{
}


DataProvider& C::operator=(const DataProvider& rhs)
{
    impl_ = rhs.impl_;
    return *this;
}


void C::GetDataObject(IDataObject** d)
{
    impl_->AddRef();
    *d = impl_;
}


void C::addFormat(IFormatProvider& fp)
{
    impl_->addFormat(fp);
}


void C::SendOnStop()
{
    impl_->SendOnStop();
}


void C::SendOnDataChange()
{
    impl_->SendOnDataChange();
}


bool detectInProcess(IUnknown* obj)
{
    IDetectInProcess* dip = 0;

    HRESULT res = obj->QueryInterface(__uuidof(IDetectInProcess),
        reinterpret_cast<void**>(&dip));

    // IDetectInProcess is not marshalable, so we get an error return value
    // if obj lives in another process as the caller.

    if (res == S_OK)
    {
        dip->Release();
        return true;
    }
    else
        return false;
}


void C::Impl::SendOnDataChange()
{
    changed_ = true;

    for (auto fp : formatProviders_)
        fp->ClearCache();

    GetAdviseHolder()->SendOnDataChange(&GIDataObject_, 0, 0);
}


void C::Impl::SendOnStop()
{
    if (changed_)
        GetAdviseHolder()->SendOnDataChange(&GIDataObject_, 0, ADVF_DATAONSTOP);
}


void C::Impl::addFormat(IFormatProvider& fp)
{
    formatProviders_.push_back(&fp);
}


auto C::Impl::GetAdviseHolder() -> IDataAdviseHolder*
{
    if (not adviseHolder_)
    {
        HRESULT res = ::CreateDataAdviseHolder(&adviseHolder_);
        D1_ASSERT(SUCCEEDED(res));
    }
    return adviseHolder_;
}


HRESULT C::Impl::QueryInterface(REFIID riid, void** ppvObject)
{
    if (not ppvObject)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::QueryInterface(" << riid << ")");

    if (riid == IID_IUnknown)
        *ppvObject = &GIDataObject_;
    else if (riid == IID_IDataObject)
        *ppvObject = &GIDataObject_;
    else if (riid == __uuidof(IDetectInProcess))
        *ppvObject = static_cast<IDetectInProcess*>(this);
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


HRESULT C::Impl::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
{
    if (not pformatetcIn or not pmedium)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::GetData(" << *pformatetcIn << ")");

    if (pformatetcIn->lindex != -1)
        return DV_E_LINDEX;

    if ((pformatetcIn->dwAspect != DVASPECT_CONTENT) and
        (pformatetcIn->dwAspect != DVASPECT_THUMBNAIL) and
        (pformatetcIn->dwAspect != DVASPECT_ICON) and
        (pformatetcIn->dwAspect != DVASPECT_DOCPRINT))
    {
        return DV_E_DVASPECT;
    }

    for (auto fp : formatProviders_)
    {
        for (int index = 0; index < fp->GetFormatCount(); ++index)
        {
            FORMATETC fetc;
            fp->GetFormat(index, fetc);

            if ((fetc.cfFormat == pformatetcIn->cfFormat) and
                // ignore fetc.ptd because we support all devices
                (fetc.dwAspect == pformatetcIn->dwAspect) and
                ((fetc.tymed & pformatetcIn->tymed) != 0))
            {
                HRESULT res = fp->GetData(index, pformatetcIn, pmedium);
                D1_TRACE("  -> " << PrintHRESULT(res));
                return res;
            }
        }
    }

    return DV_E_FORMATETC;
}


HRESULT C::Impl::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium)
{
    if (not pformatetc or not pmedium)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::GetDataHere(" << *pformatetc << ", " << *pmedium << ")");

    if (pformatetc->lindex != -1)
        return DV_E_LINDEX;

    if ((pformatetc->dwAspect != DVASPECT_CONTENT) and
        (pformatetc->dwAspect != DVASPECT_THUMBNAIL) and
        (pformatetc->dwAspect != DVASPECT_ICON) and
        (pformatetc->dwAspect != DVASPECT_DOCPRINT))
    {
        return DV_E_DVASPECT;
    }

    if ((pformatetc->tymed != TYMED_ISTORAGE) and
        (pformatetc->tymed != TYMED_ISTREAM) and
        (pformatetc->tymed != TYMED_HGLOBAL) and
        (pformatetc->tymed != TYMED_FILE))
    {
        return DV_E_TYMED;
    }

    for (auto fp : formatProviders_)
    {
        for (int index = 0; index < fp->GetFormatCount(); ++index)
        {
            FORMATETC fetc;
            fp->GetFormat(index, fetc);

            if ((fetc.cfFormat == pformatetc->cfFormat) and
                // ignore fetc.ptd because we support all devices
                (fetc.dwAspect == pformatetc->dwAspect) and
                ((fetc.tymed & pformatetc->tymed) != 0))
            {
                HRESULT res = fp->GetDataHere(index, pformatetc, pmedium);
                D1_TRACE("  -> " << PrintHRESULT(res));
                return res;
            }
        }
    }

    return DV_E_FORMATETC;
}


HRESULT C::Impl::QueryGetData(FORMATETC* pformatetc)
{
    if (not pformatetc)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::QueryGetData(" << *pformatetc << ")");

    if (pformatetc->lindex != -1)
        return DV_E_LINDEX;

    if ((pformatetc->dwAspect != DVASPECT_CONTENT) and
        (pformatetc->dwAspect != DVASPECT_THUMBNAIL) and
        (pformatetc->dwAspect != DVASPECT_ICON) and
        (pformatetc->dwAspect != DVASPECT_DOCPRINT))
    {
        return DV_E_DVASPECT;
    }

    for (auto fp : formatProviders_)
    {
        for (int index = 0; index < fp->GetFormatCount(); ++index)
        {
            FORMATETC fetc;
            fp->GetFormat(index, fetc);

            if ((fetc.cfFormat == pformatetc->cfFormat) and
                // ignore fetc.ptd because we support all devices
                (fetc.dwAspect == pformatetc->dwAspect) and
                ((fetc.tymed & pformatetc->tymed) != 0))
            {
                return S_OK;
            }
        }
    }

    return DV_E_FORMATETC;
}


HRESULT C::Impl::GetCanonicalFormatEtc(
    FORMATETC* pformatetcIn, FORMATETC* pformatetcOut)
{
    if (not pformatetcIn or not pformatetcOut)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::GetCanonicalFormatEtc(" << *pformatetcIn << ")");

    *pformatetcOut = *pformatetcIn;
    pformatetcOut->ptd = 0;

    return S_OK;
}


HRESULT C::Impl::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{
    if (not ppenumFormatEtc)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::EnumFormatEtc(" << dwDirection << ")");

    if (dwDirection != DATADIR_GET)
    {
        return E_NOTIMPL;
    }
    else
    {
        *ppenumFormatEtc = new DataProviderImplEnum(
            cbegin(formatProviders_),
            cend(formatProviders_),
            cbegin(formatProviders_),
            0);

        (*ppenumFormatEtc)->AddRef();

        return S_OK;
    }
}


HRESULT C::Impl::DAdvise(
    FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
    if (not pformatetc or not pAdvSink)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::DAdvise(" << *pformatetc
                                      << ", " << PrintADVF(advf) << ")");

    return GetAdviseHolder()->Advise(&GIDataObject_,
        pformatetc, advf, pAdvSink, pdwConnection);
}


HRESULT C::Impl::DUnadvise(DWORD dwConnection)
{
    D1_TRACE("DataProvider::DUnadvise(" << dwConnection << ")");
    return GetAdviseHolder()->Unadvise(dwConnection);
}


HRESULT C::Impl::EnumDAdvise(IEnumSTATDATA** ppenumAdvise)
{
    if (not ppenumAdvise)
        return E_INVALIDARG;

    D1_TRACE("DataProvider::EnumDAdvise");
    return GetAdviseHolder()->EnumAdvise(ppenumAdvise);
}

}
