/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

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
