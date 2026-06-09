/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.Messages;

import WinUtil.Messages;

namespace Editor
{

export class DeferredSetFocusMsg: public WinUtil::Message::Wrapper
{
public:
    DeferredSetFocusMsg(WinUtil::Message&);
    static UINT getMsgId(); // message constant
    static void post(HWND window);
};


export class LicenseInfoMsg: public WinUtil::Message::Wrapper
{
public:
    LicenseInfoMsg(WinUtil::Message&);
    bool evaluationMode() const;
    static UINT getMsgId(); // message constant
    static void post(HWND window, bool EvalMode);
};


export class ShowEvalBoxMsg: public WinUtil::Message::Wrapper
{
public:
    ShowEvalBoxMsg(WinUtil::Message&);
    static UINT getMsgId(); // message constant
    static void post(HWND window);
};

}

module : private;


import WinUtil.PrivateMessage;


namespace Editor
{

DeferredSetFocusMsg::DeferredSetFocusMsg(WinUtil::Message& m):
    Wrapper{ m, getMsgId() }
{
}


UINT DeferredSetFocusMsg::getMsgId()
{
    static unsigned int m = WinUtil::PrivateMessage::instance().getNumber();
    return m;
}


void DeferredSetFocusMsg::post(HWND window)
{
    D1_VERIFY(::PostMessage(window, getMsgId(), 0, 0));
}


LicenseInfoMsg::LicenseInfoMsg(WinUtil::Message& m):
    Wrapper{ m, getMsgId() }
{
}


UINT LicenseInfoMsg::getMsgId()
{
    static auto m = WinUtil::PrivateMessage::instance().getNumber();
    return m;
}


bool LicenseInfoMsg::evaluationMode() const
{
    return getWParam() ? true : false;
}


void LicenseInfoMsg::post(HWND window, bool EvalMode)
{
    D1_VERIFY(::PostMessage(window, getMsgId(), EvalMode, 0));
}


ShowEvalBoxMsg::ShowEvalBoxMsg(WinUtil::Message& m):
    Wrapper{ m, getMsgId() }
{
}


UINT ShowEvalBoxMsg::getMsgId()
{
    static auto m = WinUtil::PrivateMessage::instance().getNumber();
    return m;
}


void ShowEvalBoxMsg::post(HWND window)
{
    D1_VERIFY(::PostMessage(window, getMsgId(), 0, 0));
}

}
