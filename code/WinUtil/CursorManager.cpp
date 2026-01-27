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
    HCURSOR waitCursor_{};

    UniqueHandle quitEvent_;
    UniqueHandle switchOffEvent_;
    UniqueHandle timer_;
    UniqueHandle thread_;
    DWORD creatorThreadId_ = {};

    CriticalSection::Object CSO_;
    HCURSOR cursor_ = {};
    bool waitCursorSet_ = false;

    LARGE_INTEGER waitTime_ = {};

    void setWaitCursor();
    void restoreCursor();
    void set(HCURSOR newCursor) final;
    void restoreCursorState() final;

    HCURSOR getCursor() const final { return cursor_; }
    HCURSOR getWaitCursor() const final { return waitCursor_; }

    bool start() final;
    bool stop() final;

    static DWORD WINAPI threadRoutine(void*);

public:
    Impl();
    ~Impl();
};


//
//   The CursorManager thread.
//
DWORD WINAPI Impl::threadRoutine(void* cmImpl)
{
    Impl* This = static_cast<Impl*>(cmImpl);

    // this is needed, because we need to change the cursor of the main
    // thread.
    D1_VERIFY(::AttachThreadInput(
        GetCurrentThreadId(),
        This->creatorThreadId_,
        TRUE));

    const HANDLE WaitToSwitchOn[2] = {
        This->quitEvent_.get(), This->timer_.get()
    };

    const HANDLE WaitToSwitchOff[2] = {
        This->quitEvent_.get(), This->switchOffEvent_.get()
    };

    for (;;)
    {
        // This thread sleeps inside the WaitForMultipleObjects() below
        // as long as the following condition is met:
        // (quitEvent_ has not been signaled)
        // AND ( (the main thread is not in a lengthy operation section)
        //       OR
        //       (the main thread IS in a lengthy operation section AND the
        //        timer has expired)
        //     )
        DWORD res = WaitForMultipleObjects(2, WaitToSwitchOn, FALSE, INFINITE);

        D1_ASSERT(res != WAIT_FAILED);

        if (res == WAIT_OBJECT_0)
            return 0; // quitEvent_ has been signaled. Shut down thread.

        // ## we are in a lengthy operation AND the timer has expired.

        This->setWaitCursor();

        // let's wait until the lengthy operation is finished (which is
        // signaled by switchOffEvent_) or we must shut down (signaled by
        // quitEvent_).
        res = WaitForMultipleObjects(2, WaitToSwitchOff, FALSE, INFINITE);

        D1_ASSERT(res != WAIT_FAILED);

        if (res == WAIT_OBJECT_0)
            return 0; // quitEvent_ has been signaled. Shut down thread.

        // ## the lengthy operation is finished

        This->restoreCursor();
    }

    return 1; // we will never reach this
}


void Impl::setWaitCursor()
{
    auto criticalSection = CriticalSection{ CSO_ };
    if (not waitCursorSet_)
    {
        waitCursorSet_ = true;
        cursor_ = ::SetCursor(waitCursor_);
    }
}


void Impl::restoreCursor()
{
    auto criticalSection = CriticalSection{ CSO_ };
    if (waitCursorSet_)
    {
        waitCursorSet_ = false;
        ::SetCursor(cursor_);
    }
}


void Impl::set(HCURSOR newCursor)
{
    auto criticalSection = CriticalSection{ CSO_ };
    cursor_ = newCursor;
    if (not waitCursorSet_)
        ::SetCursor(newCursor);
}


void Impl::restoreCursorState()
{
    auto criticalSection = CriticalSection{ CSO_ };
    if (waitCursorSet_)
        ::SetCursor(waitCursor_);
    else if (cursor_)
        ::SetCursor(cursor_);
}


bool Impl::start()
{
    if (itIsRunning)
        return false;

    D1_VERIFY(::ResetEvent(switchOffEvent_.get()));
    D1_VERIFY(::SetWaitableTimer(
        timer_.get(),
        &waitTime_,
        0, // not periodic
        0, // no completion routine
        0, // no arguments for completion routine
        FALSE));

    itIsRunning = true;

    return true;
}


bool Impl::stop()
{
    if (not itIsRunning)
        return false;

    D1_VERIFY(::CancelWaitableTimer(timer_.get()));
    D1_VERIFY(::SetEvent(switchOffEvent_.get()));

    itIsRunning = false;

    return true;
}


Impl::Impl():
    creatorThreadId_{ ::GetCurrentThreadId() }
{
    D1_ASSERT(creatorThreadId_);

    waitTime_.QuadPart = WaitBeforeWaitCursor;

    waitCursor_ = static_cast<HCURSOR>(::LoadImage(
        0, // hinst
        IDC_WAIT,
        IMAGE_CURSOR,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
    D1_ASSERT(waitCursor_);


    quitEvent_.reset(::CreateEvent(
        0,     // default security descriptor
        TRUE,  // manual reset
        FALSE, // initially nonsignaled,
        0      // unnamed
        ));
    D1_ASSERT(quitEvent_);


    switchOffEvent_.reset(::CreateEvent(
        0,    // default security descriptor
        TRUE, // manual reset
        TRUE, // initially signaled,
        0     // unnamed
        ));
    D1_ASSERT(switchOffEvent_);


    timer_.reset(::CreateWaitableTimer(
        0,     // default security descriptor
        FALSE, // automatic reset (synchronization timer)
        0      // unnamed
        ));
    D1_ASSERT(timer_);


    DWORD threadId = 0;
    thread_.reset(::CreateThread(
        0,
        0,
        threadRoutine,
        this,
        0, // run immediately
        &threadId));
    D1_ASSERT(thread_);

    D1_VERIFY(::SetThreadPriority(
        thread_.get(),
        THREAD_PRIORITY_TIME_CRITICAL));
}


Impl::~Impl()
{
    // signal the CursorManager thread to shut down
    D1_VERIFY(::SetEvent(quitEvent_.get()));

    // sleep until CursorManager thread is shut down
    ::WaitForSingleObject(thread_.get(), INFINITE);
}

using C = CursorManager;
}


C::WaitCursorSwitch::WaitCursorSwitch()
{
}


C::WaitCursorSwitch::~WaitCursorSwitch()
{
    if (isOn_)
        instance().stop();
}


void C::WaitCursorSwitch::on()
{
    if (not isOn_)
        isOn_ = instance().start();
}


void C::WaitCursorSwitch::off()
{
    if (isOn_)
    {
        instance().stop();
        isOn_ = false;
    }
}


C::ImmediateWaitCursor::ImmediateWaitCursor():
    oldCursor_{ instance().getCursor() }
{
    instance().set(instance().getWaitCursor());
}


C::ImmediateWaitCursor::~ImmediateWaitCursor()
{
    if (oldCursor_)
        instance().set(oldCursor_);
}

auto C::instance() -> CursorManager&
{
    static Impl instance;
    return instance;
}

}
