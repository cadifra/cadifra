/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.GIClasses;

import WinUtil.GuardedFunctionCall;


namespace WinUtil
{

export template <class Base = IUnknown>
class GIUnknown: public Base
{
    // Delegates every call to itsTarget

    IUnknown& itsTarget;

public:
    GIUnknown(IUnknown& target):
        itsTarget{ target }
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject)
    {
        return GuardedComFunctionCall::GFC_2P<IUnknown, HRESULT, REFIID, void**>(
            "WinUtil::GIUnknown::QueryInterface",
            itsTarget, riid, ppvObject, E_UNEXPECTED, &IUnknown::QueryInterface)();
    }

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return GuardedComFunctionCall::GFC_0P<IUnknown, ULONG>(
            "WinUtil::GIUnknown::AddRef",
            itsTarget, 0, &IUnknown::AddRef)();
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        return GuardedComFunctionCall::GFC_0P<IUnknown, ULONG>(
            "WinUtil::GIUnknown::Release",
            itsTarget, 0, &IUnknown::Release)();
    }
};


export template <class Base = IClassFactory>
class GIClassFactory: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IClassFactory& itsTarget;

public:
    GIClassFactory(IClassFactory& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
        /* [unique][in] */ IUnknown* pUnkOuter,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject)
    {
        return GuardedComFunctionCall::GFC_3P<
            IClassFactory, HRESULT, IUnknown*, REFIID, void**>(
            "WinUtil::GIClassFactory::CreateInstance",
            itsTarget, pUnkOuter, riid, ppvObject, E_UNEXPECTED,
            &IClassFactory::CreateInstance)();
    }

    /* [local] */ HRESULT STDMETHODCALLTYPE LockServer(
        /* [in] */ BOOL fLock)
    {
        return GuardedComFunctionCall::GFC_1P<IClassFactory, HRESULT, BOOL>(
            "WinUtil::GIClassFactory::LockServer",
            itsTarget, fLock, E_UNEXPECTED, &IClassFactory::LockServer)();
    }
};


export template <class Base = IDataObject>
class GIDataObject: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IDataObject& itsTarget;

public:
    GIDataObject(IDataObject& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
        /* [unique][in] */ FORMATETC* pformatetcIn,
        /* [out] */ STGMEDIUM* pmedium)
    {
        return GuardedComFunctionCall::GFC_2P<
            IDataObject, HRESULT, FORMATETC*, STGMEDIUM*>(
            "WinUtil::GIDataObject::GetData",
            itsTarget, pformatetcIn, pmedium, E_UNEXPECTED, &IDataObject::GetData)();
    }

    /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [out][in] */ STGMEDIUM* pmedium)
    {
        return GuardedComFunctionCall::GFC_2P<
            IDataObject, HRESULT, FORMATETC*, STGMEDIUM*>(
            "WinUtil::GIDataObject::GetDataHere",
            itsTarget, pformatetc, pmedium, E_UNEXPECTED, &IDataObject::GetDataHere)();
    }

    HRESULT STDMETHODCALLTYPE QueryGetData(
        /* [unique][in] */ FORMATETC* pformatetc)
    {
        return GuardedComFunctionCall::GFC_1P<IDataObject, HRESULT, FORMATETC*>(
            "WinUtil::GIDataObject::QueryGetData",
            itsTarget, pformatetc, E_UNEXPECTED, &IDataObject::QueryGetData)();
    }

    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        /* [unique][in] */ FORMATETC* pformatectIn,
        /* [out] */ FORMATETC* pformatetcOut)
    {
        return GuardedComFunctionCall::GFC_2P<
            IDataObject, HRESULT, FORMATETC*, FORMATETC*>(
            "WinUtil::GIDataObject::GetCanonicalFormatEtc",
            itsTarget, pformatectIn, pformatetcOut, E_UNEXPECTED,
            &IDataObject::GetCanonicalFormatEtc)();
    }

    /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [unique][in] */ STGMEDIUM* pmedium,
        /* [in] */ BOOL fRelease)
    {
        return GuardedComFunctionCall::GFC_3P<
            IDataObject, HRESULT, FORMATETC*, STGMEDIUM*, BOOL>(
            "WinUtil::GIDataObject::SetData",
            itsTarget, pformatetc, pmedium, fRelease, E_UNEXPECTED,
            &IDataObject::SetData)();
    }

    HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        /* [in] */ DWORD dwDirection,
        /* [out] */ IEnumFORMATETC** ppenumFormatEtc)
    {
        return GuardedComFunctionCall::GFC_2P<
            IDataObject, HRESULT, DWORD, IEnumFORMATETC**>(
            "WinUtil::GIDataObject::EnumFormatEtc",
            itsTarget, dwDirection, ppenumFormatEtc, E_UNEXPECTED,
            &IDataObject::EnumFormatEtc)();
    }

    HRESULT STDMETHODCALLTYPE DAdvise(
        /* [in] */ FORMATETC* pformatetc,
        /* [in] */ DWORD advf,
        /* [unique][in] */ IAdviseSink* pAdvSink,
        /* [out] */ DWORD* pdwConnection)
    {
        return GuardedComFunctionCall::GFC_4P<
            IDataObject, HRESULT, FORMATETC*, DWORD, IAdviseSink*, DWORD*>(
            "WinUtil::GIDataObject::DAdvise",
            itsTarget, pformatetc, advf, pAdvSink, pdwConnection,
            E_UNEXPECTED, &IDataObject::DAdvise)();
    }

    HRESULT STDMETHODCALLTYPE DUnadvise(
        /* [in] */ DWORD dwConnection)
    {
        return GuardedComFunctionCall::GFC_1P<IDataObject, HRESULT, DWORD>(
            "WinUtil::GIDataObject::DUnadvise",
            itsTarget, dwConnection, E_UNEXPECTED, &IDataObject::DUnadvise)();
    }

    HRESULT STDMETHODCALLTYPE EnumDAdvise(
        /* [out] */ IEnumSTATDATA** ppenumAdvise)
    {
        return GuardedComFunctionCall::GFC_1P<IDataObject, HRESULT, IEnumSTATDATA**>(
            "WinUtil::GIDataObject::EnumDAdvise",
            itsTarget, ppenumAdvise, E_UNEXPECTED, &IDataObject::EnumDAdvise)();
    }
};


export template <class Base = IEnumFORMATETC>
class GIEnumFORMATETC: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IEnumFORMATETC& itsTarget;

public:
    GIEnumFORMATETC(IEnumFORMATETC& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    /* [local] */ HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ ULONG celt,
        /* [length_is][size_is][out] */ FORMATETC* rgelt,
        /* [out] */ ULONG* pceltFetched)
    {
        return GuardedComFunctionCall::GFC_3P<
            IEnumFORMATETC, HRESULT, ULONG, FORMATETC*, ULONG*>(
            "WinUtil::GIEnumFORMATETC::Next",
            itsTarget, celt, rgelt, pceltFetched, E_UNEXPECTED,
            &IEnumFORMATETC::Next)();
    }

    HRESULT STDMETHODCALLTYPE Skip(
        /* [in] */ ULONG celt)
    {
        return GuardedComFunctionCall::GFC_1P<
            IEnumFORMATETC, HRESULT, ULONG>(
            "WinUtil::GIEnumFORMATETC::Skip",
            itsTarget, celt, E_UNEXPECTED, &IEnumFORMATETC::Skip)();
    }

    HRESULT STDMETHODCALLTYPE Reset()
    {
        return GuardedComFunctionCall::GFC_0P<IEnumFORMATETC, HRESULT>(
            "WinUtil::GIEnumFORMATETC::Reset",
            itsTarget, E_UNEXPECTED, &IEnumFORMATETC::Reset)();
    }

    HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ IEnumFORMATETC** ppenum)
    {
        return GuardedComFunctionCall::GFC_1P<
            IEnumFORMATETC, HRESULT, IEnumFORMATETC**>(
            "WinUtil::GIEnumFORMATETC::Clone",
            itsTarget, ppenum, E_UNEXPECTED, &IEnumFORMATETC::Clone)();
    }
};


export template <class Base = IExternalConnection>
class GIExternalConnection: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IExternalConnection& itsTarget;

public:
    GIExternalConnection(IExternalConnection& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    DWORD STDMETHODCALLTYPE AddConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved)
    {
        return GuardedComFunctionCall::GFC_2P<
            IExternalConnection, DWORD, DWORD, DWORD>(
            "WinUtil::GIExternalConnection::AddConnection",
            itsTarget, extconn, reserved, 0,
            &IExternalConnection::AddConnection)();
    }

    DWORD STDMETHODCALLTYPE ReleaseConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved,
        /* [in] */ BOOL fLastReleaseCloses)
    {
        return GuardedComFunctionCall::GFC_3P<
            IExternalConnection, DWORD, DWORD, DWORD, BOOL>(
            "WinUtil::GIExternalConnection::ReleaseConnection",
            itsTarget, extconn, reserved, fLastReleaseCloses, 0,
            &IExternalConnection::ReleaseConnection)();
    }
};


export template <class Base = IOleObject>
class GIOleObject: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IOleObject& itsTarget;

public:
    GIOleObject(IOleObject& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    HRESULT STDMETHODCALLTYPE SetClientSite(
        /* [unique][in] */ IOleClientSite* pClientSite)
    {
        return GuardedComFunctionCall::GFC_1P<
            IOleObject, HRESULT, IOleClientSite*>(
            "WinUtil::GIOleObject::SetClientSite",
            itsTarget, pClientSite, E_UNEXPECTED,
            &IOleObject::SetClientSite)();
    }

    HRESULT STDMETHODCALLTYPE GetClientSite(
        /* [out] */ IOleClientSite** ppClientSite)
    {
        return GuardedComFunctionCall::GFC_1P<
            IOleObject, HRESULT, IOleClientSite**>(
            "WinUtil::GIOleObject::GetClientSite",
            itsTarget, ppClientSite, E_UNEXPECTED,
            &IOleObject::GetClientSite)();
    }

    HRESULT STDMETHODCALLTYPE SetHostNames(
        /* [in] */ LPCOLESTR szContainerApp,
        /* [unique][in] */ LPCOLESTR szContainerObj)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, LPCOLESTR, LPCOLESTR>(
            "WinUtil::GIOleObject::SetHostNames",
            itsTarget, szContainerApp, szContainerObj, E_UNEXPECTED,
            &IOleObject::SetHostNames)();
    }

    HRESULT STDMETHODCALLTYPE Close(
        /* [in] */ DWORD dwSaveOption)
    {
        return GuardedComFunctionCall::GFC_1P<IOleObject, HRESULT, DWORD>(
            "WinUtil::GIOleObject::Close",
            itsTarget, dwSaveOption, E_UNEXPECTED, &IOleObject::Close)();
    }

    HRESULT STDMETHODCALLTYPE SetMoniker(
        /* [in] */ DWORD dwWhichMoniker,
        /* [unique][in] */ IMoniker* pmk)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, IMoniker*>(
            "WinUtil::GIOleObject::SetMoniker",
            itsTarget, dwWhichMoniker, pmk, E_UNEXPECTED,
            &IOleObject::SetMoniker)();
    }

    HRESULT STDMETHODCALLTYPE GetMoniker(
        /* [in] */ DWORD dwAssign,
        /* [in] */ DWORD dwWhichMoniker,
        /* [out] */ IMoniker** ppmk)
    {
        return GuardedComFunctionCall::GFC_3P<
            IOleObject, HRESULT, DWORD, DWORD, IMoniker**>(
            "WinUtil::GIOleObject::GetMoniker",
            itsTarget, dwAssign, dwWhichMoniker, ppmk, E_UNEXPECTED,
            &IOleObject::GetMoniker)();
    }

    HRESULT STDMETHODCALLTYPE InitFromData(
        /* [unique][in] */ IDataObject* pDataObject,
        /* [in] */ BOOL fCreation,
        /* [in] */ DWORD dwReserved)
    {
        return GuardedComFunctionCall::GFC_3P<
            IOleObject, HRESULT, IDataObject*, BOOL, DWORD>(
            "WinUtil::GIOleObject::InitFromData",
            itsTarget, pDataObject, fCreation, dwReserved, E_UNEXPECTED,
            &IOleObject::InitFromData)();
    }

    HRESULT STDMETHODCALLTYPE GetClipboardData(
        /* [in] */ DWORD dwReserved,
        /* [out] */ IDataObject** ppDataObject)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, IDataObject**>(
            "WinUtil::GIOleObject::GetClipboardData",
            itsTarget, dwReserved, ppDataObject, E_UNEXPECTED,
            &IOleObject::GetClipboardData)();
    }

    HRESULT STDMETHODCALLTYPE DoVerb(
        /* [in] */ LONG iVerb,
        /* [unique][in] */ LPMSG lpmsg,
        /* [unique][in] */ IOleClientSite* pActiveSite,
        /* [in] */ LONG lindex,
        /* [in] */ HWND hwndParent,
        /* [unique][in] */ LPCRECT lprcPosRect)
    {
        return GuardedComFunctionCall::GFC_6P<
            IOleObject, HRESULT, LONG, LPMSG, IOleClientSite*, LONG, HWND, LPCRECT>(
            "WinUtil::GIOleObject::DoVerb",
            itsTarget, iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect,
            E_UNEXPECTED, &IOleObject::DoVerb)();
    }

    HRESULT STDMETHODCALLTYPE EnumVerbs(
        /* [out] */ IEnumOLEVERB** ppEnumOleVerb)
    {
        return GuardedComFunctionCall::GFC_1P<IOleObject, HRESULT, IEnumOLEVERB**>(
            "WinUtil::GIOleObject::EnumVerbs",
            itsTarget, ppEnumOleVerb, E_UNEXPECTED, &IOleObject::EnumVerbs)();
    }

    HRESULT STDMETHODCALLTYPE Update()
    {
        return GuardedComFunctionCall::GFC_0P<IOleObject, HRESULT>(
            "WinUtil::GIOleObject::Update",
            itsTarget, E_UNEXPECTED, &IOleObject::Update)();
    }

    HRESULT STDMETHODCALLTYPE IsUpToDate()
    {
        return GuardedComFunctionCall::GFC_0P<IOleObject, HRESULT>(
            "WinUtil::GIOleObject::IsUpToDate",
            itsTarget, E_UNEXPECTED, &IOleObject::IsUpToDate)();
    }

    HRESULT STDMETHODCALLTYPE GetUserClassID(
        /* [out] */ CLSID* pClsid)
    {
        return GuardedComFunctionCall::GFC_1P<IOleObject, HRESULT, CLSID*>(
            "WinUtil::GIOleObject::GetUserClassID",
            itsTarget, pClsid, E_UNEXPECTED, &IOleObject::GetUserClassID)();
    }

    HRESULT STDMETHODCALLTYPE GetUserType(
        /* [in] */ DWORD dwFormOfType,
        /* [out] */ LPOLESTR* pszUserType)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, LPOLESTR*>(
            "WinUtil::GIOleObject::GetUserType",
            itsTarget, dwFormOfType, pszUserType, E_UNEXPECTED,
            &IOleObject::GetUserType)();
    }

    HRESULT STDMETHODCALLTYPE SetExtent(
        /* [in] */ DWORD dwDrawAspect,
        /* [in] */ SIZEL* psizel)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, SIZEL*>(
            "WinUtil::GIOleObject::SetExtent",
            itsTarget, dwDrawAspect, psizel, E_UNEXPECTED,
            &IOleObject::SetExtent)();
    }

    HRESULT STDMETHODCALLTYPE GetExtent(
        /* [in] */ DWORD dwDrawAspect,
        /* [out] */ SIZEL* psizel)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, SIZEL*>(
            "WinUtil::GIOleObject::GetExtent",
            itsTarget, dwDrawAspect, psizel, E_UNEXPECTED,
            &IOleObject::GetExtent)();
    }

    HRESULT STDMETHODCALLTYPE Advise(
        /* [unique][in] */ IAdviseSink* pAdvSink,
        /* [out] */ DWORD* pdwConnection)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, IAdviseSink*, DWORD*>(
            "WinUtil::GIOleObject::Advise",
            itsTarget, pAdvSink, pdwConnection, E_UNEXPECTED,
            &IOleObject::Advise)();
    }

    HRESULT STDMETHODCALLTYPE Unadvise(
        /* [in] */ DWORD dwConnection)
    {
        return GuardedComFunctionCall::GFC_1P<IOleObject, HRESULT, DWORD>(
            "WinUtil::GIOleObject::Unadvise",
            itsTarget, dwConnection, E_UNEXPECTED, &IOleObject::Unadvise)();
    }

    HRESULT STDMETHODCALLTYPE EnumAdvise(
        /* [out] */ IEnumSTATDATA** ppenumAdvise)
    {
        return GuardedComFunctionCall::GFC_1P<
            IOleObject, HRESULT, IEnumSTATDATA**>(
            "WinUtil::GIOleObject::EnumAdvise",
            itsTarget, ppenumAdvise, E_UNEXPECTED,
            &IOleObject::EnumAdvise)();
    }

    HRESULT STDMETHODCALLTYPE GetMiscStatus(
        /* [in] */ DWORD dwAspect,
        /* [out] */ DWORD* pdwStatus)
    {
        return GuardedComFunctionCall::GFC_2P<
            IOleObject, HRESULT, DWORD, DWORD*>(
            "WinUtil::GIOleObject::GetMiscStatus",
            itsTarget, dwAspect, pdwStatus, E_UNEXPECTED,
            &IOleObject::GetMiscStatus)();
    }

    HRESULT STDMETHODCALLTYPE SetColorScheme(
        /* [in] */ LOGPALETTE* pLogpal)
    {
        return GuardedComFunctionCall::GFC_1P<IOleObject, HRESULT, LOGPALETTE*>(
            "WinUtil::GIOleObject::SetColorScheme",
            itsTarget, pLogpal, E_UNEXPECTED, &IOleObject::SetColorScheme)();
    }
};


template <class Base = IPersist>
class GIPersist: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IPersist& itsTarget;

public:
    GIPersist(IPersist& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }

    HRESULT STDMETHODCALLTYPE GetClassID(
        /* [out] */ CLSID* pClassID)
    {
        return GuardedComFunctionCall::GFC_1P<IPersist, HRESULT, CLSID*>(
            "WinUtil::GIPersist::GetClassID",
            itsTarget, pClassID, E_UNEXPECTED, &IPersist::GetClassID)();
    }
};


export template <class Base = IPersistFile>
class GIPersistFile: public GIPersist<Base>
{
    // Delegates every call to itsTarget

    IPersistFile& itsTarget;

public:
    GIPersistFile(IPersistFile& target):
        GIPersist<Base>{ target }, itsTarget{ target }
    {
    }

    HRESULT STDMETHODCALLTYPE IsDirty()
    {
        return GuardedComFunctionCall::GFC_0P<IPersistFile, HRESULT>(
            "WinUtil::GIPersistFile::IsDirty",
            itsTarget, E_UNEXPECTED, &IPersistFile::IsDirty)();
    }

    HRESULT STDMETHODCALLTYPE Load(
        /* [in] */ LPCOLESTR pszFileName,
        /* [in] */ DWORD dwMode)
    {
        return GuardedComFunctionCall::GFC_2P<
            IPersistFile, HRESULT, LPCOLESTR, DWORD>(
            "WinUtil::GIPersistFile::Load",
            itsTarget, pszFileName, dwMode, E_UNEXPECTED,
            &IPersistFile::Load)();
    }

    HRESULT STDMETHODCALLTYPE Save(
        /* [unique][in] */ LPCOLESTR pszFileName,
        /* [in] */ BOOL fRemember)
    {
        return GuardedComFunctionCall::GFC_2P<
            IPersistFile, HRESULT, LPCOLESTR, BOOL>(
            "WinUtil::GIPersistFile::Save",
            itsTarget, pszFileName, fRemember, E_UNEXPECTED,
            &IPersistFile::Save)();
    }

    HRESULT STDMETHODCALLTYPE SaveCompleted(
        /* [unique][in] */ LPCOLESTR pszFileName)
    {
        return GuardedComFunctionCall::GFC_1P<IPersistFile, HRESULT, LPCOLESTR>(
            "WinUtil::GIPersistFile::SaveCompleted",
            itsTarget, pszFileName, E_UNEXPECTED, &IPersistFile::SaveCompleted)();
    }

    HRESULT STDMETHODCALLTYPE GetCurFile(
        /* [out] */ LPOLESTR* ppszFileName)
    {
        return GuardedComFunctionCall::GFC_1P<IPersistFile, HRESULT, LPOLESTR*>(
            "WinUtil::GIPersistFile::GetCurFile",
            itsTarget, ppszFileName, E_UNEXPECTED, &IPersistFile::GetCurFile)();
    }
};


export template <class Base = IPersistStorage>
class GIPersistStorage: public GIPersist<Base>
{
    // Delegates every call to itsTarget

    IPersistStorage& itsTarget;

public:
    GIPersistStorage(IPersistStorage& target):
        GIPersist<Base>{ target }, itsTarget{ target }
    {
    }

    HRESULT STDMETHODCALLTYPE IsDirty()
    {
        return GuardedComFunctionCall::GFC_0P<IPersistStorage, HRESULT>(
            "WinUtil::GIPersistStorage::IsDirty",
            itsTarget, E_UNEXPECTED, &IPersistStorage::IsDirty)();
    }

    HRESULT STDMETHODCALLTYPE InitNew(
        /* [unique][in] */ IStorage* pStg)
    {
        return GuardedComFunctionCall::GFC_1P<IPersistStorage, HRESULT, IStorage*>(
            "WinUtil::GIPersistStorage::InitNew",
            itsTarget, pStg, E_UNEXPECTED, &IPersistStorage::InitNew)();
    }

    HRESULT STDMETHODCALLTYPE Load(
        /* [unique][in] */ IStorage* pStg)
    {
        return GuardedComFunctionCall::GFC_1P<IPersistStorage, HRESULT, IStorage*>(
            "WinUtil::GIPersistStorage::Load",
            itsTarget, pStg, E_UNEXPECTED, &IPersistStorage::Load)();
    }

    HRESULT STDMETHODCALLTYPE Save(
        /* [unique][in] */ IStorage* pStgSave,
        /* [in] */ BOOL fSameAsLoad)
    {
        return GuardedComFunctionCall::GFC_2P<
            IPersistStorage, HRESULT, IStorage*, BOOL>(
            "WinUtil::GIPersistStorage::Save",
            itsTarget, pStgSave, fSameAsLoad, E_UNEXPECTED,
            &IPersistStorage::Save)();
    }

    HRESULT STDMETHODCALLTYPE SaveCompleted(
        /* [unique][in] */ IStorage* pStgNew)
    {
        return GuardedComFunctionCall::GFC_1P<IPersistStorage, HRESULT, IStorage*>(
            "WinUtil::GIPersistStorage::SaveCompleted",
            itsTarget, pStgNew, E_UNEXPECTED, &IPersistStorage::SaveCompleted)();
    }

    HRESULT STDMETHODCALLTYPE HandsOffStorage()
    {
        return GuardedComFunctionCall::GFC_0P<IPersistStorage, HRESULT>(
            "WinUtil::GIPersistStorage::HandsOffStorage",
            itsTarget, E_UNEXPECTED, &IPersistStorage::HandsOffStorage)();
    }
};


export template <class Base = IRunnableObject>
class GIRunnableObject: public GIUnknown<Base>
{
    // Delegates every call to itsTarget

    IRunnableObject& itsTarget;

public:
    GIRunnableObject(IRunnableObject& target):
        GIUnknown<Base>{ target }, itsTarget{ target }
    {
    }


    HRESULT STDMETHODCALLTYPE GetRunningClass(
        /* [out] */ LPCLSID lpClsid)
    {
        return GuardedComFunctionCall::GFC_1P<
            IRunnableObject, HRESULT, LPCLSID>(
            "WinUtil::GIRunnableObject::GetRunningClass",
            itsTarget, lpClsid, E_UNEXPECTED, &IRunnableObject::GetRunningClass)();
    }

    HRESULT STDMETHODCALLTYPE Run(
        /* [in] */ LPBINDCTX pbc)
    {
        return GuardedComFunctionCall::GFC_1P<
            IRunnableObject, HRESULT, LPBINDCTX>(
            "WinUtil::GIRunnableObject::Run",
            itsTarget, pbc, E_UNEXPECTED, &IRunnableObject::Run)();
    }

    /* [local] */ BOOL STDMETHODCALLTYPE IsRunning()
    {
        return GuardedComFunctionCall::GFC_0P<IRunnableObject, BOOL>(
            "WinUtil::GIRunnableObject::IsRunning",
            itsTarget, FALSE, &IRunnableObject::IsRunning)();
    }

    HRESULT STDMETHODCALLTYPE LockRunning(
        /* [in] */ BOOL fLock,
        /* [in] */ BOOL fLastUnlockCloses)
    {
        return GuardedComFunctionCall::GFC_2P<
            IRunnableObject, HRESULT, BOOL, BOOL>(
            "WinUtil::GIRunnableObject::LockRunning",
            itsTarget, fLock, fLastUnlockCloses, E_UNEXPECTED,
            &IRunnableObject::LockRunning)();
    }

    HRESULT STDMETHODCALLTYPE SetContainedObject(
        /* [in] */ BOOL fContained)
    {
        return GuardedComFunctionCall::GFC_1P<
            IRunnableObject, HRESULT, BOOL>(
            "WinUtil::GIRunnableObject::SetContainedObject",
            itsTarget, fContained, E_UNEXPECTED,
            &IRunnableObject::SetContainedObject)();
    }
};


}
