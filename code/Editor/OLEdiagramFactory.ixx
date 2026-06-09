/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include "WinUtil/d1Trace.h"

#include <Windows.h>

export module Editor.OLEdiagramFactory;

import Editor.Diagram;

import App.Clipboard;

import Store.IStorable;

import WinUtil.GIClasses;


namespace Editor
{

// This class implements the COM "Class Object" for OLEdiagram objects.
// The reference count of this class does not influence the lifetime
// of the application or any other object!
// (AddRef and Release are only dummy implementations)

export class OLEdiagramFactory:
    private ::IExternalConnection,
    private ::IClassFactory
{
public:
    OLEdiagramFactory(
        Diagram::Factory&,
        App::IClipboard&,
        HINSTANCE appInstance,
        const Store::FactoryRegistry&);

    OLEdiagramFactory(const OLEdiagramFactory&) = delete;
    OLEdiagramFactory& operator=(const OLEdiagramFactory&) = delete;

    virtual ~OLEdiagramFactory();

    //-- IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;

    //-- IClassFactory

    /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
        /* [unique][in] */ IUnknown* pUnkOuter,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE LockServer(
        /* [in] */ BOOL fLock) override;

    //-- IExternalConnection

    DWORD STDMETHODCALLTYPE AddConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved) override;

    DWORD STDMETHODCALLTYPE ReleaseConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved,
        /* [in] */ BOOL fLastReleaseCloses) override;

    //--

    void shutDown();

private:
    WinUtil::GIExternalConnection<> GIExternalConnection_ = { *this };
    WinUtil::GIClassFactory<> GIClassFactory_ = { *this };

    DWORD COMRegisteredID_ = 0;
    HINSTANCE appInstance_;

    Diagram::Factory& diagramFactory_;
    App::IClipboard& clipboard_;

    const Store::FactoryRegistry& factoryRegistry_;
};

}

module : private;


import Editor.GUIDs;

import WinUtil.Debug;
import WinUtil.WinOstream;


D1_TRACE_DEFINE_FLAG(OLE, Editor::OLEdiagramFactory)


namespace Editor
{

using C = OLEdiagramFactory;


C::OLEdiagramFactory(
    Diagram::Factory& uf,
    App::IClipboard& clipboard,
    HINSTANCE appInstance,
    const Store::FactoryRegistry& freg):

    diagramFactory_{ uf },
    clipboard_{ clipboard },
    appInstance_{ appInstance },
    factoryRegistry_{ freg }
{
    D1_TRACE("OLEdiagramFactory::OLEdiagramFactory");

    HRESULT res = ::CoRegisterClassObject(
        GUIDs::diagram(),
        &GIExternalConnection_,
        CLSCTX_LOCAL_SERVER, // | CLSCTX_REMOTE_SERVER,
        REGCLS_MULTIPLEUSE,
        &COMRegisteredID_);

    D1_ASSERT(SUCCEEDED(res));
}


C::~OLEdiagramFactory()
{
    D1_TRACE("OLEdiagramFactory::~OLEdiagramFactory");
}


HRESULT C::QueryInterface(
    REFIID riid,
    void** ppvObject)
{
    D1_TRACE("OLEdiagramFactory::QueryInterface(" << riid << ")");

    if (riid == IID_IUnknown)
        *ppvObject = &GIExternalConnection_;
    else if (riid == IID_IExternalConnection)
        *ppvObject = &GIExternalConnection_;
    else if (riid == IID_IClassFactory)
        *ppvObject = &GIClassFactory_;
    else
        return *ppvObject = 0, E_NOINTERFACE;

    AddRef();
    return S_OK;
}


ULONG C::AddRef()
{
    ULONG res = diagramFactory_.getLifeTimeSupervisor().AddRef();
    D1_TRACE("OLEdiagramFactory::AddRef() = " << res);
    return res;
}


ULONG C::Release()
{
    ULONG res = diagramFactory_.getLifeTimeSupervisor().Release();
    D1_TRACE("OLEdiagramFactory::Release() = " << res);
    return res;
}


HRESULT C::CreateInstance(
    IUnknown* pUnkOuter,
    REFIID riid,
    void** ppvObject)
{
    D1_TRACE("OLEdiagramFactory::createInstance(" << riid << ")");

    *ppvObject = 0;

    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    auto guard = App::LifeTimeSupervisor::Guard{
        diagramFactory_.getLifeTimeSupervisor().createChild()
    };

    auto ud = std::make_unique<Diagram>(
        diagramFactory_,
        *guard.get(),
        clipboard_,
        L"",
        false,
        false,
        false,
        appInstance_,
        factoryRegistry_,
        nullptr);

    Diagram* d = ud.get();

    guard.setClient(std::move(ud));

    IUnknown* u = d->getIUnknown();

    HRESULT res = u->QueryInterface(riid, ppvObject);

    if (FAILED(res))
        return res;

    diagramFactory_.addDiagram(*d);
    guard.dismiss();

    return res;
}


HRESULT C::LockServer(BOOL fLock)
{
    D1_TRACE("OLEdiagramFactory::lockServer(" << fLock << ")");

    if (fLock)
        diagramFactory_.getLifeTimeSupervisor().lock();
    else
        diagramFactory_.getLifeTimeSupervisor().unlock();

    return S_OK;
}


DWORD C::AddConnection(
    DWORD extconn,
    DWORD reserved)
{
    D1_TRACE("OLEdiagramFactory::addConnection("
             << ")");

    if (not (extconn & EXTCONN_STRONG))
        return 0;

    return diagramFactory_.getLifeTimeSupervisor().lock();
}


DWORD C::ReleaseConnection(
    DWORD extconn,
    DWORD reserved,
    BOOL fLastReleaseCloses)
{
    D1_TRACE("OLEdiagramFactory::ReleaseConnection(x,x," << fLastReleaseCloses << ")");

    if (not (extconn & EXTCONN_STRONG))
        return 0;

    return diagramFactory_.getLifeTimeSupervisor().unlock(
        fLastReleaseCloses != 0);
}


void C::shutDown()
{
    D1_TRACE("OLEdiagramFactory::ShutDown");

    D1_VERIFY(S_OK == ::CoRevokeClassObject(COMRegisteredID_));

    D1_VERIFY(S_OK == ::CoDisconnectObject(static_cast<::IClassFactory*>(this), 0));
}

}
