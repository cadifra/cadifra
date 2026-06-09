/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <sddl.h>

module App.CommandLine;

import d1.buffer;
import d1.ScopeGuard;
import d1.StackFlag;
import d1.ToLower;

import WinUtil.ListenerWindow;
import WinUtil.Timer;
import WinUtil.UniqueHandle;
import WinUtil.Messages;

import std;


/*

The CommandLine class manages two resources:
1. the right/duty to play the master role
2. the right to exclusive access the master

- The master has to execute the command line data it receives.
- For every resource exists a mutex.
- The right to access the master doesn't mean that the master exists yet.

*/


namespace App
{

namespace
{

constexpr int MaxModuleNameLength = 1024;
constexpr ULONG_PTR CommandLineDataMsgId = 1000;


std::wstring getUserSid()
{
    HANDLE token = 0;
    BOOL bres = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
    if (not token)
        return {};
    auto closeToken = d1::ScopeGuard{ [=] { CloseHandle(token); } };

    DWORD size = 0;
    bres = GetTokenInformation(token, TokenUser, 0, 0, &size);

    auto buf = std::vector<BYTE>(size);
    bres = GetTokenInformation(token, TokenUser, buf.data(), static_cast<DWORD>(buf.size()), &size);
    if (not bres)
        return {};

    auto* user = reinterpret_cast<const TOKEN_USER*>(buf.data());

    LPTSTR str = 0;
    bres = ConvertSidToStringSid(user->User.Sid, &str);
    auto freeString = d1::ScopeGuard{ [=] { LocalFree(str); } };

    return std::wstring(str);
}


std::wstring getModuleName()
{
    auto m = d1::wbuffer(MaxModuleNameLength);
    m.resize(::GetModuleFileName(0, &*m.begin(), static_cast<DWORD>(m.size())));
    std::replace(begin(m), end(m), L'\\', L'/'); // replace backslash with slash
    d1::ToLowerString<>().convert(m);
    m += L':' + getUserSid();
    return m;
}


class ReleaseMutex
{
    HANDLE mutex_;

public:
    ReleaseMutex(HANDLE m):
        mutex_{ m }
    {
    }
    ~ReleaseMutex()
    {
        if (mutex_)
            D1_VERIFY(::ReleaseMutex(mutex_));
    }
    void noRelease() { mutex_ = 0; }

    ReleaseMutex(const ReleaseMutex&) = delete;
    ReleaseMutex& operator=(const ReleaseMutex&) = delete;
};


using WindowList = std::vector<HWND>;

BOOL CALLBACK EnumWindowsProc(
    HWND hwnd,    // handle to parent window
    LPARAM lParam // application-defined value
)
{
    auto* wl = reinterpret_cast<WindowList*>(lParam);
    if (not wl)
        return FALSE;
    wl->push_back(hwnd);
    return TRUE;
}

auto getTopLevelWindows() -> std::vector<HWND>
{
    auto wl = WindowList{};
    EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&wl));
    return wl;
}


std::wstring addCurrentDirectory(const std::wstring& s)
{
    auto buf = d1::wbuffer(1);
    DWORD res = ::GetCurrentDirectory(0, &*buf.begin());
    D1_ASSERT(res);

    if (not res)
        return L"\"\"" + s;

    buf.resize(res + 1);
    res = ::GetCurrentDirectory(
        static_cast<DWORD>(buf.size()),
        &*buf.begin());

    if (not res)
        return L"\"\"" + s;

    buf.resize(res);

    return L'"' + buf + L'"' + s;
}


std::wstring consumeCurrentDirectory(const std::wstring& s)
{
    auto it = begin(s);

    if (it == end(s) or *it != L'"')
        return s;

    ++it;

    while (it != end(s) and *it != L'"')
        ++it;

    if (it == end(s))
        return s;

    const auto dir = std::wstring(begin(s) + 1, it);

    D1_VERIFY(::SetCurrentDirectory(dir.c_str()));

    ++it;

    return std::wstring(it, end(s));
}


class CopyDataClient
{
public:
    virtual void addToCmdList(const std::wstring& cmdLine) = 0;

protected:
    ~CopyDataClient() = default;
};

class CopyData
{
    using This = CopyData;
    CopyDataClient& client_;
    WinUtil::ListenerWindow listenerWindow_;
    WinUtil::ProcRegistrar procReg_;
    void onWM_COPYDATA(WinUtil::WM_COPYDATA_Msg);

public:
    CopyData(CopyDataClient&, const std::wstring& windowName);
    ~CopyData();
};


CopyData::CopyData(CopyDataClient& c, const std::wstring& windowName):
    client_{ c },
    procReg_{ listenerWindow_.getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.addAlwaysReady(&This::onWM_COPYDATA);

    D1_VERIFY(::SetWindowText(
        listenerWindow_.getWindowHandle(), windowName.c_str()));
}


CopyData::~CopyData()
{
    D1_VERIFY(::SetWindowText(listenerWindow_.getWindowHandle(), L""));
}


void CopyData::onWM_COPYDATA(WinUtil::WM_COPYDATA_Msg m)
{
    const COPYDATASTRUCT* cd = m.copyData();

    if (cd and cd->dwData == CommandLineDataMsgId and cd->cbData > 0)
    {
        DWORD charCount = cd->cbData / sizeof(std::wstring::value_type);

        auto s = d1::wbuffer(charCount);

        memcpy(&*s.begin(), cd->lpData, s.size() * sizeof(std::wstring::value_type));

        m.setResult(TRUE);
        client_.addToCmdList(s);
    }
}

using C = CommandLine;
}


class C::Impl:
    public WinUtil::Timer::Client,
    public CopyDataClient
{
    using Status = C::Status;

    const std::wstring moduleName_ = getModuleName();
    WinUtil::UniqueHandle masterRoleMutex_;
    WinUtil::UniqueHandle masterAccessMutex_;
    Status status_ = Status::Failed;
    C::IReceiver* receiver_ = nullptr;
    LifeTimeSupervisor* lifeTimeSupervisor_ = nullptr;
    std::unique_ptr<WinUtil::Timer> timer_;
    d1::StackFlag::Ref isProcessing_;
    std::wstring localCommand_;
    bool localCommandIsExecuted_ = false;
    std::unique_ptr<CopyData> copyData_;

    using CmdList = std::list<std::wstring>;
    CmdList cmdList_;

    void timerElapsed() final;
    HWND findMasterWindow() const;
    bool send();
    void trySend();
    void becomeMaster();

    void startProcessing();
    void addToCmdList(const std::wstring& commandLine) final;

public:
    Impl(const std::wstring& cmdLine);

    Status getStatus() const { return status_; }
    void set(LifeTimeSupervisor&, C::IReceiver&);
    void stopReceive();
};


C::Impl::Impl(const std::wstring& cmdLine):
    localCommand_{ cmdLine }
{
    HWND w = ::GetDesktopWindow();
    std::wostringstream masterRole, masterAccess;
    masterRole << moduleName_ << L':' << std::hex << w << L'r';
    masterAccess << moduleName_ << L':' << std::hex << w << L'a';

    masterRoleMutex_.reset(::CreateMutex(0, FALSE, masterRole.str().c_str()));
    D1_ASSERT(masterRoleMutex_);

    masterAccessMutex_.reset(::CreateMutex(0, FALSE, masterAccess.str().c_str()));
    D1_ASSERT(masterAccessMutex_);

    trySend();
}


void C::Impl::stopReceive()
{
    if (status_ != Status::Master)
        return;

    copyData_.reset();
    ::ReleaseMutex(masterRoleMutex_.get()); // ignore result
    status_ = Status::Slave;
}


void C::Impl::set(LifeTimeSupervisor& lts, C::IReceiver& r)
{
    D1_ASSERT(status_ == Status::Master);
    D1_ASSERT(not receiver_);
    D1_ASSERT(not lifeTimeSupervisor_);
    D1_ASSERT(masterAccessMutex_);
    D1_ASSERT(masterRoleMutex_);
    lifeTimeSupervisor_ = &lts;
    receiver_ = &r;
    D1_VERIFY(::ReleaseMutex(masterAccessMutex_.get()));
    startProcessing();
}


void C::Impl::addToCmdList(const std::wstring& cmdLine)
{
    lifeTimeSupervisor_->lock();
    cmdList_.push_back(cmdLine);
    startProcessing();
}


void C::Impl::startProcessing()
{
    if (receiver_ and
        (not cmdList_.empty() or not localCommandIsExecuted_) and
        not timer_.get())
    {
        // process the received command lines using low priority messages
        timer_ = std::make_unique<WinUtil::Timer>(*this, 0); // "immediate"
    }
}


void C::Impl::timerElapsed()
{
    if (isProcessing_)
        return;
    auto stackFlag = d1::StackFlag{ isProcessing_ };

    if (not localCommandIsExecuted_)
    {
        auto shutDown = d1::ScopeGuard{ [=] { lifeTimeSupervisor_->forceShutDown(); } };

        localCommandIsExecuted_ = true;
        receiver_->interpretCommandLine(localCommand_);

        shutDown.dismiss();
    }
    else if (not cmdList_.empty())
    {
        auto unlocker = d1::ScopeGuard{ [=] { lifeTimeSupervisor_->unlock(true); } };

        const auto s = std::wstring(cmdList_.front());
        cmdList_.pop_front();
        receiver_->interpretCommandLine(consumeCurrentDirectory(s));
    }

    if (cmdList_.empty() and localCommandIsExecuted_)
        timer_.reset();
}


HWND C::Impl::findMasterWindow() const
{
    const DWORD thisProcessId = ::GetCurrentProcessId();

    const auto wl = WindowList{ getTopLevelWindows() };

    for (auto w : wl)
    {
        DWORD processId = 0;
        ::GetWindowThreadProcessId(w, &processId);

        if (processId == thisProcessId)
            continue;

        if (::GetWindowTextLength(w) != static_cast<int>(moduleName_.size()))
            continue;
        auto mn = d1::wbuffer(moduleName_.size() + 1);

        mn.resize(
            ::GetWindowText(
                w,
                &*mn.begin(),
                static_cast<int>(mn.size())));

        if (mn != moduleName_)
            continue;

        return w;
    }

    return 0;
}


bool C::Impl::send()
{
    HWND masterWindow = findMasterWindow();

    if (not masterWindow)
        return false;

    ::SetForegroundWindow(masterWindow);

    const auto s = addCurrentDirectory(localCommand_);

    auto cd = COPYDATASTRUCT{
        .dwData = CommandLineDataMsgId,
        .cbData = static_cast<DWORD>(s.size() * sizeof(std::wstring::value_type)),
        .lpData = const_cast<std::wstring::value_type*>(s.c_str())
    };

    LRESULT res = ::SendMessage(masterWindow, WM_COPYDATA, 0,
        reinterpret_cast<LPARAM>(&cd));

    return res ? true : false;
}


void C::Impl::becomeMaster()
{
    D1_ASSERT(status_ != Status::Master);
    status_ = Status::Master;

    copyData_ = std::make_unique<CopyData>(*this, moduleName_);
}


void C::Impl::trySend()
{
    DWORD waitRes = ::WaitForSingleObject(masterAccessMutex_.get(), INFINITE);

    switch (waitRes)
    {
    case WAIT_ABANDONED: // fall through
    case WAIT_OBJECT_0:  // we got master access
        break;

    case WAIT_TIMEOUT: // fall through
    default:
        status_ = Status::Failed;
        return;
    }
    auto masterAccess = ReleaseMutex{ masterAccessMutex_.get() };


    // ask if there is a master
    waitRes = ::WaitForSingleObject(masterRoleMutex_.get(), 0); // return immediately

    switch (waitRes)
    {
    case WAIT_TIMEOUT: // there is a master
        status_ = Status::Slave;
        break;

    case WAIT_ABANDONED: // fall through
    case WAIT_OBJECT_0:  // we got the master role
        becomeMaster();
        masterAccess.noRelease();
        return;

    default:
        status_ = Status::Failed;
        return;
    }

    if (send())
        return;

    // Probably, the master has shut down in the meantime.
    // Now we expect to become the master.
    waitRes = ::WaitForSingleObject(masterRoleMutex_.get(), INFINITE);

    switch (waitRes)
    {
    case WAIT_ABANDONED: // fall through
    case WAIT_OBJECT_0:  // we got the master role
        becomeMaster();
        masterAccess.noRelease();
        return;

    case WAIT_TIMEOUT: // fall through
    default:
        status_ = Status::Failed;
        return;
    }
}


C::CommandLine(const std::wstring& cmdLine):
    impl_{ std::make_unique<Impl>(cmdLine) }
{
}


C::~CommandLine() = default;


auto C::getStatus() const -> Status
{
    return impl_->getStatus();
}


void C::set(LifeTimeSupervisor& lts, IReceiver& r)
{
    impl_->set(lts, r);
}


void C::stopReceive()
{
    impl_->stopReceive();
}

}
