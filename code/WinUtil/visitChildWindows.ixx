/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.visitChildWindows;

namespace WinUtil
{

export class ChildWindowVisitor
{
public:
    virtual bool visit(HWND hwnd) = 0;

protected:
    ~ChildWindowVisitor() = default;
};

export void visitChildWindows(HWND parent, ChildWindowVisitor* Visitor);
// For every child window of parent, the member function Visitor->Visit
// is called with the child window handle as parameter.
// The Visit member function has to return false if it wants to stop
// the visiting process.


// Useful example of a ChildWindowVisitor:
//
export class ChildWindowSender: public ChildWindowVisitor
{
    UINT msg_;
    WPARAM WParam_;
    LPARAM LParam_;
    bool visit(HWND hwnd) override; // calls SendMessage for hwnd
public:
    ChildWindowSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

}


module : private;


namespace WinUtil
{

BOOL CALLBACK enumChildProcForVisitor(HWND hwnd, LPARAM lParam)
{
    return reinterpret_cast<ChildWindowVisitor*>(lParam)->visit(hwnd);
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
