/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

module WinUtil.MessageLoop;

import WinUtil.Debug;
import WinUtil.GuardedFunctionCall;

import std;


#ifdef _DEBUG
namespace
{
const bool CheckInMessageLoop =
    0 != WinUtil::DebugEnv::inst().getInt("Heap", "CheckInMessageLoop");
}
#endif



namespace WinUtil
{

namespace
{
using C = MessageLoop;
}


C::IPreProc::IPreProc()
{
}


void C::IPreProc::set(MessageLoop& ml)
{
    if (messageLoop_)
        return;
    ml.add(*this);
    messageLoop_ = &ml;
}


void C::IPreProc::clear()
{
    if (messageLoop_)
        messageLoop_->forget(*this);
    messageLoop_ = 0;
}


C::IOneTimePostProc::IOneTimePostProc()
{
}


void C::IOneTimePostProc::set(MessageLoop& ml)
{
    if (messageLoop_)
        return;
    ml.add(*this);
    messageLoop_ = &ml;
}


void C::IOneTimePostProc::clear()
{
    if (messageLoop_)
        messageLoop_->forget(*this);
    messageLoop_ = 0;
}


C::IIdleProc::IIdleProc()
{
}


void C::IIdleProc::set(MessageLoop& ml)
{
    if (messageLoop_)
        return;
    ml.add(*this);
    messageLoop_ = &ml;
}


void C::IIdleProc::clear()
{
    if (messageLoop_)
        messageLoop_->forget(*this);
    messageLoop_ = 0;
}


C::MessageLoop()
{
}


namespace
{
void unregisterAll(auto& v)
{
    for (auto* e : v)
    {
        if (e)
            e->clear();
    }
}
}


C::~MessageLoop()
{
    unregisterAll(preProc_);
    unregisterAll(oneTimePostProc_);
    unregisterAll(idleProc_);
}


namespace
{
template <typename V>
void doRegister(V& set, V& waitSet, typename V::value_type p)
{
    D1_ASSERT(end(set) == std::ranges::find(set, p));
    D1_ASSERT(end(waitSet) == std::ranges::find(waitSet, p));

    waitSet.push_back(p);
}
}


void C::add(IPreProc& p)
{
    doRegister(preProc_, waitForInsertPreProc_, &p);
}


void C::add(IOneTimePostProc& p)
{
    doRegister(oneTimePostProc_, waitForInsertOneTimePostProc_, &p);
}


void C::add(IIdleProc& p)
{
    doRegister(idleProc_, waitForInsertIdleProc_, &p);
}


namespace
{
template <typename V>
void doUnregister(V& set, V& waitSet, typename V::value_type p)
{
    auto i = std::ranges::find(set, p);

    if (end(set) != i)
        *i = 0;

    i = std::ranges::find(waitSet, p);

    if (end(waitSet) != i)
        waitSet.erase(i);
}
}


void C::forget(IPreProc& p)
{
    doUnregister(preProc_, waitForInsertPreProc_, &p);
}


void C::forget(IOneTimePostProc& p)
{
    doUnregister(oneTimePostProc_, waitForInsertOneTimePostProc_, &p);
}


void C::forget(IIdleProc& p)
{
    doUnregister(idleProc_, waitForInsertIdleProc_, &p);
}


namespace
{
template <typename V>
void join(V& set, V& waitSet)
{
    if (waitSet.empty())
        return;
    set.insert(begin(set), rbegin(waitSet), rend(waitSet));
    waitSet.clear();
}
}


bool C::callPreProc(MSG& m)
{
    join(preProc_, waitForInsertPreProc_);

    for (auto i = begin(preProc_); i != end(preProc_); /* no ++i */)
    {
        if (*i)
        {
            if ((*i)->preProcess(m))
                return true;
            ++i;
        }
        else
            i = preProc_.erase(i);
    }
    return false;
}


void C::callOneTimePostProc()
{
    join(oneTimePostProc_, waitForInsertOneTimePostProc_);

    for (auto i = begin(oneTimePostProc_); i != end(oneTimePostProc_); /* no ++i */)
    {
        if (*i)
        {
            IOneTimePostProc* p = *i;
            p->clear(); // *i becomes 0
            p->postProcess();
        }
        i = oneTimePostProc_.erase(i);
    }
}


void C::callIdleProc()
{
    join(idleProc_, waitForInsertIdleProc_);

    for (auto i = begin(idleProc_); i != end(idleProc_); /* no ++i */)
    {
        if (*i)
        {
            (*i)->idleProcess();
            ++i;
        }
        else
            i = idleProc_.erase(i);
    }
}


int C::doLoop()
{
    const std::pair<bool, int> defRes(false, 0);
    // continue loop in case of an exception

    for (;;)
    {
        auto res = GuardedCallHelpers::call(
            "WinUtil::MessageLoop", *this, &MessageLoop::loopImpl, defRes);

        if (res.first)
            return res.second;
    }
    return 0;
}


auto C::loopImpl() -> std::pair<bool, int>
// returns (true, exitCode) to stop the loop or (false, 0) to continue the loop
{
#ifdef _DEBUG
    if (CheckInMessageLoop)
        _CrtCheckMemory();
#endif

    auto msg = MSG{};

    while (not idleProc_.empty() or not waitForInsertIdleProc_.empty())
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
            break;
        callIdleProc();
    }

    BOOL res{ ::GetMessage(&msg, 0, 0, 0) };

    if (res == -1)
        return { true, -1 }; // ## GetMessage failure, exit

    if (res == 0)
        return { true, static_cast<int>(msg.wParam) }; // WM_QUIT, exit

    if (callPreProc(msg))
        return { false, 0 }; // discard this msg

    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);

    callOneTimePostProc();

    return { false, 0 };
}


void C::processMessages()
{
    auto msg = MSG{};
    while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(static_cast<int>(msg.wParam));
            return;
        }

        if (callPreProc(msg))
            continue;

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        callOneTimePostProc();
    }
}

}
