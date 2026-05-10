/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <objidl.h>

export module App.Com;

import d1.AutoComPtr;
import d1.Observer;
import d1.RefCounter;

import WinUtil.GIClasses;
import WinUtil.IWindow;

import std;


namespace App
{

class __declspec(uuid("CB3EB24F-3736-4b4f-9B34-983B21A3A061"))
IDetectInProcess: public IUnknown
{
    // no methods
};


export bool detectInProcess(IUnknown* obj);
// Returns true if "obj" lives in the same process as the caller
// and implements "IDetectInProcess".


export class IFormatProvider
{
public:
    void AddRef();
    void Release();

    virtual int GetFormatCount() const { return 1; }

    virtual void GetFormat(int formatIndex, tagFORMATETC&) const = 0;

    virtual HRESULT GetData(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) = 0;

    virtual HRESULT GetDataHere(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) = 0;

    virtual void ClearCache() {}

    virtual ~IFormatProvider() = default;

private:
    int refCount_ = 0;
};


export auto createEmbedSourceProvider(::IPersistStorage*)
    -> d1::AutoComPtr<IFormatProvider>;
// The caller is responsible to delete the created object.
// The object will not AddRef the IPersistStorage interface
// ("reference only").


export class DataProvider // has value semantic
{
public:
    DataProvider();
    ~DataProvider();
    DataProvider(const DataProvider&);
    DataProvider& operator=(const DataProvider&);

    void GetDataObject(IDataObject**);

    void addFormat(IFormatProvider&); // does an addref

    void SendOnStop();
    void SendOnDataChange();

private:
    class Impl;

    d1::AutoComPtr<Impl> impl_;
};


using FormatProviders =
    std::vector<d1::AutoComPtr<App::IFormatProvider>>;


class DataProvider::Impl:
    public IDataObject,
    public IDetectInProcess
{
    WinUtil::GIDataObject<> GIDataObject_ = *this;
    d1::RefCounter<Impl> refCounter_;
    d1::AutoComPtr<IDataAdviseHolder> adviseHolder_;
    bool changed_ = false;

    FormatProviders formatProviders_;

    IDataAdviseHolder* GetAdviseHolder();

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
};


class DataProviderImplEnum: public ::IEnumFORMATETC
{
    using FP = std::vector<d1::AutoComPtr<App::IFormatProvider>>;

    WinUtil::GIEnumFORMATETC<> GIEnumFORMATETC_ = *this;
    d1::RefCounter<DataProviderImplEnum> refCounter_;

    const FP::const_iterator start_, stop_;
    FP::const_iterator current_;
    int currentIndex_;

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
};


export class IFormatExtractor
{
public:
    virtual bool extract(::IDataObject*) = 0;

protected:
    ~IFormatExtractor() = default;
};


export class IFormatTester
{
public:
    virtual bool QueryExtract(::IDataObject*) = 0;

protected:
    ~IFormatTester() = default;
};


export class IDataHolder
{
public:
    virtual ~IDataHolder() = default;

    virtual bool extract(IFormatExtractor&) = 0;
    virtual bool QueryExtract(IFormatTester&) = 0;
};


// DataHolder is a wrapper that makes an IDataHolder from
// an IDataObject (COM).
// DataHolder has value semantics.

export class DataHolder: public IDataHolder
{
public:
    DataHolder();
    DataHolder(::IDataObject*);

    DataHolder(const DataHolder&);
    DataHolder& operator=(const DataHolder&);

    DataHolder(DataHolder&&);
    DataHolder& operator=(DataHolder&&);

    bool extract(IFormatExtractor&) override;
    bool QueryExtract(IFormatTester&) override;

private:
    d1::AutoComPtr<::IDataObject> dataObject_;
};


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
//  by using the function App::ReferenceFormatHandler::AddFormatProvider.
//  See header App/ReferenceFormatHandler.h
//
//  The same header contains functions to test, whether an ::IDataObject
//  contains an IDataObjectReferenced pointer (QueryExtract) and to extract
//  an IDataObjectReferenced pointer from ::IDataObject (Extract).
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

bool QueryExtract(IDataHolder& dh);
// If dh contains a pointer to an IDataObjectReferenced residing in
// the process of the same application instance as the calling thread,
// QueryExtract returns true. Otherwise, QueryExtract returns false.
// QueryExtract guaranties that IDataObjectReferenced pointers can only be
// transferred via a windows ::IDataObject if the provider and the
// consumer reside in the same process.

auto extract(IDataHolder& dh) -> IDataObjectReferenced*;
// If dh contains a pointer to an IDataObjectReferenced of the same
// application instance, Extract returns 'x'. Otherwise Extract returns 0.
}


export namespace TextFormatHandler
{
void addFormatProvider(
    DataProvider& dp, const std::wstring& t, LCID lcid = LOCALE_USER_DEFAULT);
// Adds a new FormatProviders that contains the text t into
// the DataProvider object dp.

bool QueryExtract(IDataHolder& dh);
// Returns true if dh contains a text format.

void extract(IDataHolder& dh, std::wostream& o);
// If dh contains a text, the function writes the text into o

std::wstring extract(IDataHolder& dh);
// Returns the text that is in the clipboard dh or an empty string
// if there is no text in dh.
}


export class IClipboard: public IDataHolder
{
public:
    virtual void put(DataProvider&) = 0;
    // Puts the IDataObject from the DataProvider into the clipboard
    // if the clipboard isn't used by somebody else.

    virtual ~IClipboard() = default;
    IClipboard() = default;
    IClipboard(const IClipboard&) = delete;
    IClipboard& operator=(const IClipboard&) = delete;
};


export class Clipboard: public IClipboard
{
    ::IDataObject* lastPutObject_ = nullptr;

public:
    Clipboard() = default;
    virtual ~Clipboard();

    virtual void put(DataProvider&);
    virtual bool extract(IFormatExtractor&);
    virtual bool QueryExtract(IFormatTester&);
};


export class IClipboardChange
{
public:
    virtual void clipboardChanged() = 0;

protected:
    ~IClipboardChange() = default;
};


export class ClipboardViewer
{
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    ClipboardViewer(WinUtil::Window&);
    ~ClipboardViewer();

    auto getConnector() -> d1::Observer::C<IClipboardChange>;
};

}
