/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"
#include "d1Trace.h"

#include <Windows.h>
#include <oledlg.h>

module WinUtil.MessageLoop;

import WinUtil.Debug;
import WinUtil.WinOstream;


D1_TRACE_DEFINE_FLAG(OLE, WinUtil::MessageFilter)


namespace WinUtil
{

namespace
{
using C = MessageFilter;
}


class C::Impl: private ::IMessageFilter
{
    ::IMessageFilter* previousFilter_ = nullptr;
    HWND window_ = {};
    bool itIsRegistered = false;
    ULONG refCount_ = 0; // dummy
    int postponeIncomingCalls_ = 0;

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

    void activate(HWND);
    void deactivate();
    void incPostponeIncomingCalls();
    void decPostponeIncomingCalls();
};


C::Impl::~Impl()
{
    deactivate();
}


void C::Impl::activate(HWND w)
{
    window_ = w;
    if (not itIsRegistered)
    {
        itIsRegistered = true;
        D1_VERIFY(SUCCEEDED(
            ::CoRegisterMessageFilter(this, &previousFilter_)));
    }
}


void C::Impl::deactivate()
{
    if (itIsRegistered)
    {
        ::IMessageFilter* dummy = 0;
        D1_VERIFY(SUCCEEDED(
            ::CoRegisterMessageFilter(previousFilter_, &dummy)));
        itIsRegistered = false;
    }
}


void C::Impl::incPostponeIncomingCalls()
{
    ++postponeIncomingCalls_;
}


void C::Impl::decPostponeIncomingCalls()
{
    --postponeIncomingCalls_;
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
    return ++refCount_;
}


ULONG C::Impl::Release()
{
    return --refCount_;
}

// MessageFilter::Impl::IMessageFilter functions:

DWORD C::Impl::HandleInComingCall(
    DWORD dwCallType,
    HTASK htaskCaller,
    DWORD dwTickCount,
    LPINTERFACEINFO lpInterfaceInfo)
{
    HRESULT hres = postponeIncomingCalls_ > 0
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
            ob.hWndOwner = window_;
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

MessageFilter& C::instance()
{
    static MessageFilter Instance;
    return Instance;
}


void C::activate(HWND w)
{
    impl_->activate(w);
}


void C::deactivate()
{
    impl_->deactivate();
}


C::MessageFilter():
    impl_{ std::make_unique<MessageFilter::Impl>() }
{
}


C::~MessageFilter() = default;


void C::incPostponeIncomingCalls()
{
    impl_->incPostponeIncomingCalls();
}


void C::decPostponeIncomingCalls()
{
    impl_->decPostponeIncomingCalls();
}

}
