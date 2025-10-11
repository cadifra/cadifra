/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

module WinUtil;

import std;


#ifdef _DEBUG
namespace
{
const bool CheckInMessageLoop =
    0 != WinUtil::DebugEnv::Inst().GetInt("Heap", "CheckInMessageLoop");
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


void C::IPreProc::Register(MessageLoop& ml)
{
    if (itsMessageLoop)
        return;
    ml.Register(*this);
    itsMessageLoop = &ml;
}


void C::IPreProc::Unregister()
{
    if (itsMessageLoop)
        itsMessageLoop->Unregister(*this);
    itsMessageLoop = 0;
}


C::IOneTimePostProc::IOneTimePostProc()
{
}


void C::IOneTimePostProc::Register(MessageLoop& ml)
{
    if (itsMessageLoop)
        return;
    ml.Register(*this);
    itsMessageLoop = &ml;
}


void C::IOneTimePostProc::Unregister()
{
    if (itsMessageLoop)
        itsMessageLoop->Unregister(*this);
    itsMessageLoop = 0;
}


C::IIdleProc::IIdleProc()
{
}


void C::IIdleProc::Register(MessageLoop& ml)
{
    if (itsMessageLoop)
        return;
    ml.Register(*this);
    itsMessageLoop = &ml;
}


void C::IIdleProc::Unregister()
{
    if (itsMessageLoop)
        itsMessageLoop->Unregister(*this);
    itsMessageLoop = 0;
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
            e->Unregister();
    }
}
}


C::~MessageLoop()
{
    unregisterAll(itsPreProc);
    unregisterAll(itsOneTimePostProc);
    unregisterAll(itsIdleProc);
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


void C::Register(IPreProc& p)
{
    doRegister(itsPreProc, itsWaitForInsertPreProc, &p);
}


void C::Register(IOneTimePostProc& p)
{
    doRegister(itsOneTimePostProc, itsWaitForInsertOneTimePostProc, &p);
}


void C::Register(IIdleProc& p)
{
    doRegister(itsIdleProc, itsWaitForInsertIdleProc, &p);
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


void C::Unregister(IPreProc& p)
{
    doUnregister(itsPreProc, itsWaitForInsertPreProc, &p);
}


void C::Unregister(IOneTimePostProc& p)
{
    doUnregister(itsOneTimePostProc, itsWaitForInsertOneTimePostProc, &p);
}


void C::Unregister(IIdleProc& p)
{
    doUnregister(itsIdleProc, itsWaitForInsertIdleProc, &p);
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


bool C::CallPreProc(MSG& m)
{
    join(itsPreProc, itsWaitForInsertPreProc);

    for (auto i = begin(itsPreProc); i != end(itsPreProc); /* no ++i */)
    {
        if (*i)
        {
            if ((*i)->PreProcess(m))
                return true;
            ++i;
        }
        else
            i = itsPreProc.erase(i);
    }
    return false;
}


void C::CallOneTimePostProc()
{
    join(itsOneTimePostProc, itsWaitForInsertOneTimePostProc);

    for (auto i = begin(itsOneTimePostProc); i != end(itsOneTimePostProc); /* no ++i */)
    {
        if (*i)
        {
            IOneTimePostProc* p = *i;
            p->Unregister(); // *i becomes 0
            p->PostProcess();
        }
        i = itsOneTimePostProc.erase(i);
    }
}


void C::CallIdleProc()
{
    join(itsIdleProc, itsWaitForInsertIdleProc);

    for (auto i = begin(itsIdleProc); i != end(itsIdleProc); /* no ++i */)
    {
        if (*i)
        {
            (*i)->IdleProcess();
            ++i;
        }
        else
            i = itsIdleProc.erase(i);
    }
}


int C::DoLoop()
{
    const std::pair<bool, int> defRes(false, 0);
    // continue loop in case of an exception

    for (;;)
    {
        auto res = GuardedCallHelpers::call(
            "WinUtil::MessageLoop", *this, &MessageLoop::LoopImpl, defRes);

        if (res.first)
            return res.second;
    }
    return 0;
}


auto C::LoopImpl() -> std::pair<bool, int>
// returns (true, exitCode) to stop the loop or (false, 0) to continue the loop
{
#ifdef _DEBUG
    if (CheckInMessageLoop)
        _CrtCheckMemory();
#endif

    auto msg = MSG{};

    while (!itsIdleProc.empty() || !itsWaitForInsertIdleProc.empty())
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
            break;
        CallIdleProc();
    }

    BOOL res{ ::GetMessage(&msg, 0, 0, 0) };

    if (res == -1)
        return { true, -1 }; // ## GetMessage failure, exit

    if (res == 0)
        return { true, static_cast<int>(msg.wParam) }; // WM_QUIT, exit

    if (CallPreProc(msg))
        return { false, 0 }; // discard this msg

    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);

    CallOneTimePostProc();

    return { false, 0 };
}


void C::ProcessMessages()
{
    auto msg = MSG{};
    while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(static_cast<int>(msg.wParam));
            return;
        }

        if (CallPreProc(msg))
            continue;

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        CallOneTimePostProc();
    }
}

}
