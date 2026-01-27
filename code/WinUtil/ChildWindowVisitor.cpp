/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.Window;

namespace WinUtil
{

namespace
{

BOOL CALLBACK enumChildProcForVisitor(HWND hwnd, LPARAM lParam)
{
    return reinterpret_cast<ChildWindowVisitor*>(lParam)->visit(hwnd);
}

}

void visitChildWindows(HWND parent, ChildWindowVisitor* Visitor)
{
    D1_VERIFY(::EnumChildWindows(parent, enumChildProcForVisitor,
        reinterpret_cast<LPARAM>(Visitor)));
}


// class ChildWindowSender:

ChildWindowSender::ChildWindowSender(UINT uMsg, WPARAM wParam, LPARAM lParam):
    msg_{ uMsg }, WParam_{ wParam }, LParam_{ lParam }
{
}

bool ChildWindowSender::visit(HWND hwnd)
{
    ::SendMessage(hwnd, msg_, WParam_, LParam_);
    return true;
}

}
