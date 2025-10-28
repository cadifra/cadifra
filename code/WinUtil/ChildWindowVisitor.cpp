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

BOOL CALLBACK EnumChildProcForVisitor(HWND hwnd, LPARAM lParam)
{
    return reinterpret_cast<ChildWindowVisitor*>(
        lParam)
        ->Visit(hwnd);
}

}

void VisitChildWindows(HWND parent, ChildWindowVisitor* Visitor)
{
    D1_VERIFY(::EnumChildWindows(parent, EnumChildProcForVisitor,
        reinterpret_cast<LPARAM>(Visitor)));
}


// class ChildWindowSender:

ChildWindowSender::ChildWindowSender(UINT uMsg, WPARAM wParam, LPARAM lParam):
    itsMsg{ uMsg }, itsWParam{ wParam }, itsLParam{ lParam }
{
}

bool ChildWindowSender::Visit(HWND hwnd)
{
    ::SendMessage(hwnd, itsMsg, itsWParam, itsLParam);
    return true;
}

}
