/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

// Note:
//
// This module uses the API CreateWaitableTimer().
//
// By not using CreateWaitableTimer, we would have been forced to make a
// context switch to the Cursor Manager thread every time a lengthy operation
// section is entered. Thanks to CreateWaitableTimer, this is not necessary
// and a context switch to the CursorManager thread occurs only in a
// lengthy operation, when the timer has expired.

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.CursorManager;

import WinUtil.CriticalSection;
import WinUtil.UniqueHandle;


namespace WinUtil
{

namespace
{

constexpr LONGLONG WaitBeforeWaitCursor = -300 * 10000;
// time in 100ns, negative means relative


class Impl: public CursorManager
{
    bool itIsRunning = false;
    HCURSOR itsWaitCursor{};

    UniqueHandle itsQuitEvent;
    UniqueHandle itsSwitchOffEvent;
    UniqueHandle itsTimer;
    UniqueHandle itsThread;
    DWORD itsCreatorThreadId = {};

    CriticalSection::Object itsCSO;
    HCURSOR itsCursor = {};
    bool itsWaitCursorSet = false;

    LARGE_INTEGER itsWaitTime = {};

    void SetWaitCursor();
    void RestoreCursor();
    void Set(HCURSOR newCursor) final;
    void RestoreCursorState() final;

    HCURSOR GetCursor() const final { return itsCursor; }
    HCURSOR GetWaitCursor() const final { return itsWaitCursor; }

    bool Start() final;
    bool Stop() final;

    static DWORD WINAPI ThreadRoutine(void*);

public:
    Impl();
    ~Impl();
};


//
//   The CursorManager thread.
//
DWORD WINAPI Impl::ThreadRoutine(void* cmImpl)
{
    Impl* This = static_cast<Impl*>(cmImpl);

    // this is needed, because we need to change the cursor of the main
    // thread.
    D1_VERIFY(::AttachThreadInput(
        GetCurrentThreadId(),
        This->itsCreatorThreadId,
        TRUE));

    const HANDLE WaitToSwitchOn[2] = {
        This->itsQuitEvent.get(), This->itsTimer.get()
    };

    const HANDLE WaitToSwitchOff[2] = {
        This->itsQuitEvent.get(), This->itsSwitchOffEvent.get()
    };

    for (;;)
    {
        // This thread sleeps inside the WaitForMultipleObjects() below
        // as long as the following condition is met:
        // (itsQuitEvent has not been signaled)
        // AND ( (the main thread is not in a lengthy operation section)
        //       OR
        //       (the main thread IS in a lengthy operation section AND the
        //        timer has expired)
        //     )
        DWORD res = WaitForMultipleObjects(2, WaitToSwitchOn, FALSE, INFINITE);

        D1_ASSERT(res != WAIT_FAILED);

        if (res == WAIT_OBJECT_0)
            return 0; // itsQuitEvent has been signaled. Shut down thread.

        // ## we are in a lengthy operation AND the timer has expired.

        This->SetWaitCursor();

        // let's wait until the lengthy operation is finished (which is
        // signaled by itsSwitchOffEvent) or we must shut down (signaled by
        // itsQuitEvent).
        res = WaitForMultipleObjects(2, WaitToSwitchOff, FALSE, INFINITE);

        D1_ASSERT(res != WAIT_FAILED);

        if (res == WAIT_OBJECT_0)
            return 0; // itsQuitEvent has been signaled. Shut down thread.

        // ## the lengthy operation is finished

        This->RestoreCursor();
    }

    return 1; // we will never reach this
}


void Impl::SetWaitCursor()
{
    auto criticalSection = CriticalSection{ itsCSO };
    if (!itsWaitCursorSet)
    {
        itsWaitCursorSet = true;
        itsCursor = ::SetCursor(itsWaitCursor);
    }
}


void Impl::RestoreCursor()
{
    auto criticalSection = CriticalSection{ itsCSO };
    if (itsWaitCursorSet)
    {
        itsWaitCursorSet = false;
        ::SetCursor(itsCursor);
    }
}


void Impl::Set(HCURSOR newCursor)
{
    auto criticalSection = CriticalSection{ itsCSO };
    itsCursor = newCursor;
    if (!itsWaitCursorSet)
        ::SetCursor(newCursor);
}


void Impl::RestoreCursorState()
{
    auto criticalSection = CriticalSection{ itsCSO };
    if (itsWaitCursorSet)
        ::SetCursor(itsWaitCursor);
    else if (itsCursor)
        ::SetCursor(itsCursor);
}


bool Impl::Start()
{
    if (itIsRunning)
        return false;

    D1_VERIFY(::ResetEvent(itsSwitchOffEvent.get()));
    D1_VERIFY(::SetWaitableTimer(
        itsTimer.get(),
        &itsWaitTime,
        0, // not periodic
        0, // no completion routine
        0, // no arguments for completion routine
        FALSE));

    itIsRunning = true;

    return true;
}


bool Impl::Stop()
{
    if (!itIsRunning)
        return false;

    D1_VERIFY(::CancelWaitableTimer(itsTimer.get()));
    D1_VERIFY(::SetEvent(itsSwitchOffEvent.get()));

    itIsRunning = false;

    return true;
}


Impl::Impl():
    itsCreatorThreadId{ ::GetCurrentThreadId() }
{
    D1_ASSERT(itsCreatorThreadId);

    itsWaitTime.QuadPart = WaitBeforeWaitCursor;

    itsWaitCursor = static_cast<HCURSOR>(::LoadImage(
        0, // hinst
        IDC_WAIT,
        IMAGE_CURSOR,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
    D1_ASSERT(itsWaitCursor);


    itsQuitEvent.reset(::CreateEvent(
        0,     // default security descriptor
        TRUE,  // manual reset
        FALSE, // initially nonsignaled,
        0      // unnamed
        ));
    D1_ASSERT(itsQuitEvent);


    itsSwitchOffEvent.reset(::CreateEvent(
        0,    // default security descriptor
        TRUE, // manual reset
        TRUE, // initially signaled,
        0     // unnamed
        ));
    D1_ASSERT(itsSwitchOffEvent);


    itsTimer.reset(::CreateWaitableTimer(
        0,     // default security descriptor
        FALSE, // automatic reset (synchronization timer)
        0      // unnamed
        ));
    D1_ASSERT(itsTimer);


    DWORD threadId = 0;
    itsThread.reset(::CreateThread(
        0,
        0,
        ThreadRoutine,
        this,
        0, // run immediately
        &threadId));
    D1_ASSERT(itsThread);

    D1_VERIFY(::SetThreadPriority(
        itsThread.get(),
        THREAD_PRIORITY_TIME_CRITICAL));
}


Impl::~Impl()
{
    // signal the CursorManager thread to shut down
    D1_VERIFY(::SetEvent(itsQuitEvent.get()));

    // sleep until CursorManager thread is shut down
    ::WaitForSingleObject(itsThread.get(), INFINITE);
}

using C = CursorManager;
}


C::WaitCursorSwitch::WaitCursorSwitch()
{
}


C::WaitCursorSwitch::~WaitCursorSwitch()
{
    if (itIsOn)
        Instance().Stop();
}


void C::WaitCursorSwitch::On()
{
    if (!itIsOn)
        itIsOn = Instance().Start();
}


void C::WaitCursorSwitch::Off()
{
    if (itIsOn)
    {
        Instance().Stop();
        itIsOn = false;
    }
}


C::ImmediateWaitCursor::ImmediateWaitCursor():
    itsOldCursor{ Instance().GetCursor() }
{
    Instance().Set(Instance().GetWaitCursor());
}


C::ImmediateWaitCursor::~ImmediateWaitCursor()
{
    if (itsOldCursor)
        Instance().Set(itsOldCursor);
}

auto C::Instance() -> CursorManager&
{
    static Impl instance;
    return instance;
}

}
