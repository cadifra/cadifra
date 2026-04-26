/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.OLEdiagram;

import Editor.StorageHolder;

import App.Com;
import App.ISdiApp;

import d1.AutoComPtr;
import d1.Observer;

import WinUtil.GIClasses;
import WinUtil.ISavedChange;
import WinUtil.Timer;

import Core.Main;

import std;


namespace Editor
{

namespace OLEdiagram
{

export class OUD:
    private IPersistFile,
    private IPersistStorage,
    private IDataObject,
    private IOleObject,
    private IRunnableObject,
    private IExternalConnection,
    private App::Document::INameChange,
    private WinUtil::ISavedChange,
    private WinUtil::Timer::Client,
    private Core::IDocumentChange
{
    WinUtil::GIPersistFile<> GIPersistFile_ = { *this };
    WinUtil::GIPersistStorage<> GIPersistStorage_ = { *this };
    WinUtil::GIDataObject<> GIDataObject_ = { *this };
    WinUtil::GIOleObject<> GIOleObject_ = { *this };
    WinUtil::GIExternalConnection<> GIExternalConnection_ = { *this };
    WinUtil::GIRunnableObject<> GIRunnableObject_ = { *this };

    App::IDocument& document_;
    d1::AutoComPtr<IOleClientSite> clientSite_;
    d1::AutoComPtr<IMoniker> moniker_;
    d1::AutoComPtr<IOleAdviseHolder> OleAdviseHolder_;
    d1::AutoComPtr<IDataObject> dataObject_;
    App::DataProvider dataProvider_;
    StorageHolder& storageHolder_;
    Core::IDiagram& diagram_;

    DWORD ROTid_ = 0;
    bool discardFlag_ = false;

    d1::Observer::C<WinUtil::ISavedChange> saveConnector_;
    d1::Observer::C<App::Document::INameChange> documentNameConnector_;
    d1::Observer::C<Core::IDocumentChange> documentChangeConnector_;

    WinUtil::Timer timer_ = { *this };

    bool itHasShutDown = false;
    class DeferredShutDownCall;
    std::unique_ptr<DeferredShutDownCall> deferredShutDownCall_;

public:
    //-- IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;


    //-- IExternalConnection

    DWORD STDMETHODCALLTYPE AddConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved) override;

    DWORD STDMETHODCALLTYPE ReleaseConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved,
        /* [in] */ BOOL fLastReleaseCloses) override;


    //-- IPersist

    HRESULT STDMETHODCALLTYPE GetClassID(
        /* [out] */ CLSID* pClassID) override;


    //-- IPersistFile

    HRESULT STDMETHODCALLTYPE IsDirty() override;

    HRESULT STDMETHODCALLTYPE Load(
        /* [in] */ LPCOLESTR pszFileName,
        /* [in] */ DWORD dwMode) override;

    HRESULT STDMETHODCALLTYPE Save(
        /* [unique][in] */ LPCOLESTR pszFileName,
        /* [in] */ BOOL fRemember) override;

    HRESULT STDMETHODCALLTYPE SaveCompleted(
        /* [unique][in] */ LPCOLESTR pszFileName) override;

    HRESULT STDMETHODCALLTYPE GetCurFile(
        /* [out] */ LPOLESTR* ppszFileName) override;


    //-- IPersistStorage

    // HRESULT STDMETHODCALLTYPE IsDirty();

    HRESULT STDMETHODCALLTYPE InitNew(
        /* [unique][in] */ IStorage* pStg) override;

    HRESULT STDMETHODCALLTYPE Load(
        /* [unique][in] */ IStorage* pStg) override;

    HRESULT STDMETHODCALLTYPE Save(
        /* [unique][in] */ IStorage* pStgSave,
        /* [in] */ BOOL fSameAsLoad) override;

    HRESULT STDMETHODCALLTYPE SaveCompleted(
        /* [unique][in] */ IStorage* pStgNew) override;

    HRESULT STDMETHODCALLTYPE HandsOffStorage() override;


    //-- IDataObject

    /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
        /* [unique][in] */ FORMATETC* pformatetcIn,
        /* [out] */ STGMEDIUM* pmedium) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [out][in] */ STGMEDIUM* pmedium) override;

    HRESULT STDMETHODCALLTYPE QueryGetData(
        /* [unique][in] */ FORMATETC* pformatetc) override;

    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        /* [unique][in] */ FORMATETC* pformatectIn,
        /* [out] */ FORMATETC* pformatetcOut) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
        /* [unique][in] */ FORMATETC* pformatetc,
        /* [unique][in] */ STGMEDIUM* pmedium,
        /* [in] */ BOOL fRelease) override;

    HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        /* [in] */ DWORD dwDirection,
        /* [out] */ IEnumFORMATETC** ppenumFormatEtc) override;

    HRESULT STDMETHODCALLTYPE DAdvise(
        /* [in] */ FORMATETC* pformatetc,
        /* [in] */ DWORD advf,
        /* [unique][in] */ IAdviseSink* pAdvSink,
        /* [out] */ DWORD* pdwConnection) override;

    HRESULT STDMETHODCALLTYPE DUnadvise(
        /* [in] */ DWORD dwConnection) override;

    HRESULT STDMETHODCALLTYPE EnumDAdvise(
        /* [out] */ IEnumSTATDATA** ppenumAdvise) override;


    //-- IOleObject

    HRESULT STDMETHODCALLTYPE SetClientSite(
        /* [unique][in] */ IOleClientSite* pClientSite) override;

    HRESULT STDMETHODCALLTYPE GetClientSite(
        /* [out] */ IOleClientSite** ppClientSite) override;

    HRESULT STDMETHODCALLTYPE SetHostNames(
        /* [in] */ LPCOLESTR szContainerApp,
        /* [unique][in] */ LPCOLESTR szContainerObj) override;

    HRESULT STDMETHODCALLTYPE Close(
        /* [in] */ DWORD dwSaveOption) override;

    HRESULT STDMETHODCALLTYPE SetMoniker(
        /* [in] */ DWORD dwWhichMoniker,
        /* [unique][in] */ IMoniker* pmk) override;

    HRESULT STDMETHODCALLTYPE GetMoniker(
        /* [in] */ DWORD dwAssign,
        /* [in] */ DWORD dwWhichMoniker,
        /* [out] */ IMoniker** ppmk) override;

    HRESULT STDMETHODCALLTYPE InitFromData(
        /* [unique][in] */ IDataObject* pDataObject,
        /* [in] */ BOOL fCreation,
        /* [in] */ DWORD dwReserved) override;

    HRESULT STDMETHODCALLTYPE GetClipboardData(
        /* [in] */ DWORD dwReserved,
        /* [out] */ IDataObject** ppDataObject) override;

    HRESULT STDMETHODCALLTYPE DoVerb(
        /* [in] */ LONG iVerb,
        /* [unique][in] */ LPMSG lpmsg,
        /* [unique][in] */ IOleClientSite* pActiveSite,
        /* [in] */ LONG lindex,
        /* [in] */ HWND hwndParent,
        /* [unique][in] */ LPCRECT lprcPosRect) override;

    HRESULT STDMETHODCALLTYPE EnumVerbs(
        /* [out] */ IEnumOLEVERB** ppEnumOleVerb) override;

    HRESULT STDMETHODCALLTYPE Update(void) override;

    HRESULT STDMETHODCALLTYPE IsUpToDate(void) override;

    HRESULT STDMETHODCALLTYPE GetUserClassID(
        /* [out] */ CLSID* pClsid) override;

    HRESULT STDMETHODCALLTYPE GetUserType(
        /* [in] */ DWORD dwFormOfType,
        /* [out] */ LPOLESTR* pszUserType) override;

    HRESULT STDMETHODCALLTYPE SetExtent(
        /* [in] */ DWORD dwDrawAspect,
        /* [in] */ SIZEL* psizel) override;

    HRESULT STDMETHODCALLTYPE GetExtent(
        /* [in] */ DWORD dwDrawAspect,
        /* [out] */ SIZEL* psizel) override;

    HRESULT STDMETHODCALLTYPE Advise(
        /* [unique][in] */ IAdviseSink* pAdvSink,
        /* [out] */ DWORD* pdwConnection) override;

    HRESULT STDMETHODCALLTYPE Unadvise(
        /* [in] */ DWORD dwConnection) override;

    HRESULT STDMETHODCALLTYPE EnumAdvise(
        /* [out] */ IEnumSTATDATA** ppenumAdvise) override;

    HRESULT STDMETHODCALLTYPE GetMiscStatus(
        /* [in] */ DWORD dwAspect,
        /* [out] */ DWORD* pdwStatus) override;

    HRESULT STDMETHODCALLTYPE SetColorScheme(
        /* [in] */ LOGPALETTE* pLogpal) override;


    //-- IRunnableObject

    HRESULT STDMETHODCALLTYPE GetRunningClass(
        /* [out] */ LPCLSID lpClsid) override;

    HRESULT STDMETHODCALLTYPE Run(
        /* [in] */ LPBINDCTX pbc) override;

    /* [local] */ BOOL STDMETHODCALLTYPE IsRunning() override;

    HRESULT STDMETHODCALLTYPE LockRunning(
        /* [in] */ BOOL fLock,
        /* [in] */ BOOL fLastUnlockCloses) override;

    HRESULT STDMETHODCALLTYPE SetContainedObject(
        /* [in] */ BOOL fContained) override;


    //-- IDocumentNameObserver

    void documentNameChanged(std::wstring) override;


    //-- ISavedChange

    void lastSavedAt(const _FILETIME&) override;

    //-- ITimerClient

    void timerElapsed() override;

    //-- IDocumentChange

    void documentChanged() override;

    //--

    void shutDown();

public:
    OUD(App::IDocument&, StorageHolder&, Core::IDiagram&);

    OUD(const OUD&) = delete;
    OUD& operator=(const OUD&) = delete;

    virtual ~OUD();

    IUnknown* GetIUnknown();

    void updateContainer();

private:
    void shutDownImpl();

    DWORD lock(bool lock, bool lastReleaseCloses);
    IDataObject* GetDataObject();

    // Running Object Table functions:
    void RegisterAtROT(const std::wstring&);
    void RegisterAtROT(d1::AutoComPtr<IMoniker>);
    void RevokeFromROT();
};

}

}
