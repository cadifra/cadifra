/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"
#include "d1Trace.h"

#include <Windows.h>
#include <oledlg.h>

module WinUtil;


D1_TRACE_DEFINE_FLAG(OLE, WinUtil::MessageFilter)


namespace WinUtil
{

namespace
{
using C = MessageFilter;
}


class C::Impl: private ::IMessageFilter
{
    ::IMessageFilter* itsPreviousFilter = nullptr;
    HWND itsWindow = {};
    bool itIsRegistered = false;
    ULONG itsRefCount = 0; // dummy
    int itsPostponeIncomingCalls = 0;

public:
    //-- IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) final;

    ULONG STDMETHODCALLTYPE AddRef() final;

    ULONG STDMETHODCALLTYPE Release() final;


    //-- IMessageFilter

    DWORD STDMETHODCALLTYPE HandleInComingCall(
        /* [in] */ DWORD dwCallType,
        /* [in] */ HTASK htaskCaller,
        /* [in] */ DWORD dwTickCount,
        /* [in] */ LPINTERFACEINFO lpInterfaceInfo) final;

    DWORD STDMETHODCALLTYPE RetryRejectedCall(
        /* [in] */ HTASK htaskCallee,
        /* [in] */ DWORD dwTickCount,
        /* [in] */ DWORD dwRejectType) final;

    DWORD STDMETHODCALLTYPE MessagePending(
        /* [in] */ HTASK htaskCallee,
        /* [in] */ DWORD dwTickCount,
        /* [in] */ DWORD dwPendingType) final;

    //--

public:
    virtual ~Impl();

    void Register(HWND);
    void Unregister();
    void IncPostponeIncomingCalls();
    void DecPostponeIncomingCalls();
};


C::Impl::~Impl()
{
    Unregister();
}


void C::Impl::Register(HWND w)
{
    itsWindow = w;
    if (!itIsRegistered)
    {
        itIsRegistered = true;
        D1_VERIFY(SUCCEEDED(
            ::CoRegisterMessageFilter(this, &itsPreviousFilter)));
    }
}


void C::Impl::Unregister()
{
    if (itIsRegistered)
    {
        ::IMessageFilter* dummy = 0;
        D1_VERIFY(SUCCEEDED(
            ::CoRegisterMessageFilter(itsPreviousFilter, &dummy)));
        itIsRegistered = false;
    }
}


void C::Impl::IncPostponeIncomingCalls()
{
    ++itsPostponeIncomingCalls;
}


void C::Impl::DecPostponeIncomingCalls()
{
    --itsPostponeIncomingCalls;
}

// MessageFilter::Impl::IUnknown functions:

HRESULT C::Impl::QueryInterface(
    REFIID riid,
    void** ppvObject)
{
    if (riid == IID_IUnknown)
        *ppvObject = static_cast<IUnknown*>(this);
    else if (riid == IID_IMessageFilter)
        *ppvObject = static_cast<IMessageFilter*>(this);
    else
        return *ppvObject = 0, E_NOINTERFACE;

    AddRef();
    return S_OK;
}


ULONG C::Impl::AddRef()
{
    return ++itsRefCount;
}


ULONG C::Impl::Release()
{
    return --itsRefCount;
}

// MessageFilter::Impl::IMessageFilter functions:

DWORD C::Impl::HandleInComingCall(
    DWORD dwCallType,
    HTASK htaskCaller,
    DWORD dwTickCount,
    LPINTERFACEINFO lpInterfaceInfo)
{
    HRESULT hres = itsPostponeIncomingCalls > 0
                       ? SERVERCALL_RETRYLATER
                       : SERVERCALL_ISHANDLED;

#ifdef _DEBUG
    if (debugOut)
    {
        dout << "MessageFilter: HandleInComingCall dwCallType=" << dwCallType;

        if (lpInterfaceInfo)
            dout << " pUnk=" << lpInterfaceInfo->pUnk
                 << " iid=" << lpInterfaceInfo->iid
                 << " wMethod=" << lpInterfaceInfo->wMethod;

        switch (hres)
        {
        case SERVERCALL_ISHANDLED:
            dout << " -> ISHANDLED" << std::endl;
            break;
        case SERVERCALL_REJECTED:
            dout << " -> REJECTED" << std::endl;
            break;
        case SERVERCALL_RETRYLATER:
            dout << " -> RETRYLATER" << std::endl;
            break;
        default:
            dout << " -> " << hres << std::endl;
            break;
        }
    }
#endif

    return hres;
}


DWORD C::Impl::RetryRejectedCall(
    HTASK htaskCallee,
    DWORD dwTickCount,
    DWORD dwRejectType)
{
    const DWORD RetryWaitTime = 200;
    const DWORD Patience = 3000;

    if (dwRejectType == SERVERCALL_RETRYLATER)
    {
        D1_TRACE("MessageFilterImpl::RetryRejectedCall - SERVERCALL_RETRYLATER");


        if (dwTickCount > Patience)
        {
            auto ob = OLEUIBUSY{};

            ob.cbStruct = sizeof(ob);
            ob.dwFlags = BZ_DISABLECANCELBUTTON;
            ob.hWndOwner = itsWindow;
            ob.lpszCaption = 0;
            ob.lpfnHook = 0;
            ob.lCustData = 0;
            ob.hInstance = 0;
            ob.lpszTemplate = 0;
            ob.hResource = 0;
            ob.hTask = htaskCallee;
            ob.lphWndDialog = 0;

            switch (::OleUIBusy(&ob))
            {
            case OLEUI_SUCCESS:
            case OLEUI_BZ_SWITCHTOSELECTED:
            case OLEUI_BZ_RETRYSELECTED:
            case OLEUI_BZ_CALLUNBLOCKED:
                return RetryWaitTime;

            case OLEUI_CANCEL:
                return -1;

            default:
                return RetryWaitTime;
            }
        }

        return RetryWaitTime;
    }
    else // SERVERCALL_REJECTED
    {
        D1_TRACE("MessageFilterImpl::RetryRejectedCall");
        return -1; // cancel
    }
}


DWORD C::Impl::MessagePending(
    HTASK htaskCallee,
    DWORD dwTickCount,
    DWORD dwPendingType)
{
    D1_TRACE("MessageFilterImpl::MessagePending");
    return PENDINGMSG_WAITNOPROCESS;
}

// class MessageFilter:

MessageFilter& C::Instance()
{
    static MessageFilter Instance;
    return Instance;
}


void C::Register(HWND w)
{
    itsImpl->Register(w);
}


void C::Unregister()
{
    itsImpl->Unregister();
}


C::MessageFilter():
    itsImpl{ std::make_unique<MessageFilter::Impl>() }
{
}


C::~MessageFilter() = default;


void C::IncPostponeIncomingCalls()
{
    itsImpl->IncPostponeIncomingCalls();
}


void C::DecPostponeIncomingCalls()
{
    itsImpl->DecPostponeIncomingCalls();
}

}
