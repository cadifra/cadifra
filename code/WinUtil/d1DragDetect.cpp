/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include <Windowsx.h>

#include "d1/d1verify.h"

#include <stdlib.h> // abs

module WinUtil;

import d1.ScopeGuard;

import std;

#ifdef _DEBUG
import :Debug;
#endif


#ifdef _DEBUG
#define D1_DRAG_DETECT_TEST 0 // set to 1 for testing
#endif


namespace WinUtil
{

namespace
{


#ifdef _DEBUG
bool DebugEnabled()
{
    return (WinUtil::DebugEnv::Inst().GetInt(
                "WinUtil", "d1DragDetect") != 0);
}
#endif


#if defined(_DEBUG) && D1_DRAG_DETECT_TEST
constexpr LONGLONG DRAG_DETECT_TIMEOUT = -4000 * 10000;
#else
constexpr LONGLONG DRAG_DETECT_TIMEOUT = -500 * 10000; // time in 100ns, negative means relative
#endif


enum class UpDown
{
    up = 1,
    down
};

UINT ButtonMessageCode(const d1::MouseButton mb, const UpDown ud)
{
    switch (mb)
    {
        using enum d1::MouseButton;
    case left:
        return ud == UpDown::up ? WM_LBUTTONUP : WM_LBUTTONDOWN;
    case middle:
        return ud == UpDown::up ? WM_MBUTTONUP : WM_MBUTTONDOWN;
    case right:
        return ud == UpDown::up ? WM_RBUTTONUP : WM_RBUTTONDOWN;
    default:
        D1_ASSERT(0);
        return 0;
    }
}


struct MouseState
{
    POINT pos = {}; // cursor pos, relative to client area
    bool leftButtonDown = false;
    bool middleButtonDown = false;
    bool rightButtonDown = false;

    MouseState()
    {
    }

    void EvalMouseMsg(const MSG& msg)
    {
        leftButtonDown = (msg.wParam & MK_LBUTTON) != 0;
        middleButtonDown = (msg.wParam & MK_MBUTTON) != 0;
        rightButtonDown = (msg.wParam & MK_RBUTTON) != 0;
        pos.x = GET_X_LPARAM(msg.lParam);
        pos.y = GET_Y_LPARAM(msg.lParam);
    }

    bool IsButtonDown(d1::MouseButton mb) const
    {
        switch (mb)
        {
        case d1::MouseButton::left:
            return leftButtonDown;
        case d1::MouseButton::middle:
            return middleButtonDown;
        case d1::MouseButton::right:
            return rightButtonDown;
        default:
            return false;
        }
    }
};


#ifdef _DEBUG

class TestEnlargeDragRect
{
    const int itsSaved_SM_CXDRAG;
    const int itsSaved_SM_CYDRAG;
    int itsFactor;

public:
    TestEnlargeDragRect(int factor = 8):
        itsSaved_SM_CXDRAG{ ::GetSystemMetrics(SM_CXDRAG) },
        itsSaved_SM_CYDRAG{ ::GetSystemMetrics(SM_CYDRAG) },
        itsFactor{ 1 }
    {
        EnlargeDragRect(factor);
    }
    ~TestEnlargeDragRect()
    {
        RestoreInitialDragRect();
    }
    int GetFactor() const { return itsFactor; }

private:
    static void SetDragRect(int width, int height)
    {
        D1_VERIFY(0 != ::SystemParametersInfo(SPI_SETDRAGWIDTH, width, 0, 0));
        D1_VERIFY(0 != ::SystemParametersInfo(SPI_SETDRAGHEIGHT, height, 0, 0));
    }
    void RestoreInitialDragRect() const
    {
        SetDragRect(itsSaved_SM_CXDRAG, itsSaved_SM_CYDRAG);
    }
    void EnlargeDragRect(int factor)
    {
        SetDragRect(
            factor * ::GetSystemMetrics(SM_CXDRAG),
            factor * ::GetSystemMetrics(SM_CYDRAG));
        itsFactor *= factor;
    }
};

#endif


class Imp
{
    const HWND itsWindow = {};
    const POINT itsInitPosScreen = {};
    const POINT itsInitPos = {}; // in client coordinates
    const d1::MouseButton itsButton;

    MouseState itsMouseState;

    bool itsDragDetected = false;

#ifdef _DEBUG
    const bool itsDebugEnabled{ DebugEnabled() };
#if D1_DRAG_DETECT_TEST
    TestEnlargeDragRect itsTestEnlargeDragRect;
#endif
#endif

    const int itsSM_CXDRAG;
    const int itsSM_CYDRAG;

public:
    Imp(
        HWND hwnd,      // handle to window
        POINT init_pos, // initial position in screen coordinates
        d1::MouseButton mb);

    ~Imp();

    bool Result() const { return itsDragDetected; }

    POINT GetMousePos() const { return itsMouseState.pos; }

    bool OutsideDragRectangle(const POINT& p) const;
    // returns true, if p is outside drag rectangle defined by system metrics.
    // p is in client coordinates.

    bool OutsideDragRectangle() const
    {
        return OutsideDragRectangle(GetMousePos());
    }

private:
    void Work();

    bool IsButtonDown() const
    {
        return itsMouseState.IsButtonDown(itsButton);
    }

    UINT ButtonMsgCode(UpDown ud) const
    {
        return ButtonMessageCode(itsButton, ud);
    }

#ifdef _DEBUG

    template <class T>
    void DBG(int line, const char* text, const T& t) const
    {
        if (itsDebugEnabled)
            WinUtil::dout
                << "@@@ WinUtil::d1DragDetect #" << line
                << ": " << text << t << std::endl;
    }

    void DBG(int line, const char* text) const
    {
        if (itsDebugEnabled)
            WinUtil::dout
                << "@@@ WinUtil::d1DragDetect #" << line << ": " << text << std::endl;
    }

#else

    template <class T>
    void DBG(int line, const char* text, const T& t) const
    {
    }
    void DBG(int line, const char* text) const {}

#endif
};


bool DoPostMessage(HWND hWnd, const MSG& msg)
{
    return ::PostMessage(hWnd, msg.message, msg.wParam, msg.lParam) != 0;
}


POINT ToClient(HWND hwnd, POINT pos)
{
    D1_VERIFY(::ScreenToClient(hwnd, &pos));
    return pos;
}


Imp::Imp(
    HWND hwnd, // handle to window
    POINT init_pos,
    d1::MouseButton mb):

    itsWindow{ hwnd },
    itsInitPosScreen{ init_pos },
    itsInitPos{ ToClient(hwnd, init_pos) },
    itsButton{ mb },
    itsSM_CXDRAG{ ::GetSystemMetrics(SM_CXDRAG) },
    itsSM_CYDRAG{ ::GetSystemMetrics(SM_CYDRAG) }
{
    ::SetCapture(itsWindow);
    Work();
}


Imp::~Imp()
{
    D1_VERIFY(::ReleaseCapture() != 0);

    if (itsDragDetected)
        DBG(__LINE__, "### DRAG DETECTED ###");
    else
        DBG(__LINE__, "--- finished. no drag ---");
}


bool Imp::OutsideDragRectangle(const POINT& p) const
{
    if (itsSM_CXDRAG < 2 * abs(p.x - itsInitPos.x))
        return true;

    if (itsSM_CYDRAG < 2 * abs(p.y - itsInitPos.y))
        return true;

    return false;
}


void Imp::Work()
{
    DBG(__LINE__, "started. Button=", (int)itsButton);
    DBG(__LINE__, "SM_CXDRAG=", itsSM_CXDRAG);
    DBG(__LINE__, "SM_CYDRAG=", itsSM_CYDRAG);
    if (!itsWindow)
    {
        DBG(__LINE__, "!window. returning false.");
        return;
    }

    if (itsButton == d1::MouseButton::left)
    {
        DBG(__LINE__, "calling API DragDetect");
        itsDragDetected = (::DragDetect(itsWindow, itsInitPosScreen) != 0);
        return;
    }

    HANDLE timer = ::CreateWaitableTimer(0, TRUE, 0); // manual-reset timer
    D1_ASSERT(timer);
    auto closeTimer = d1::ScopeGuard{ [=] { ::CloseHandle(timer); } };

    LARGE_INTEGER time;
    time.QuadPart = DRAG_DETECT_TIMEOUT;
    D1_VERIFY(::SetWaitableTimer(timer, &time, 0, 0, 0, FALSE));

    for (;;) // Message Loop
    {
        DBG(__LINE__, "begin of loop.");
        const DWORD wmo_res =
            ::MsgWaitForMultipleObjects(1, &timer, 0, INFINITE, QS_MOUSE | QS_KEY);
        DBG(__LINE__, "MsgWaitForMultipleObjects returned ", wmo_res);

        if (wmo_res == -1)
        {
            DBG(__LINE__, "MsgWaitForMultipleObjects returned error.");
            DWORD err = ::GetLastError();
            DBG(__LINE__, "GetLastError=", err);
            return;
        }

        if (wmo_res == WAIT_OBJECT_0)
        {
            DBG(__LINE__, "timer expired");
            itsDragDetected = true;
            return;
        }

        auto msg = MSG{};

        const UINT up_msg_id = ButtonMsgCode(UpDown::up);
        if (0 != ::PeekMessage(&msg, itsWindow, up_msg_id, up_msg_id, PM_NOREMOVE))
        {
            DBG(__LINE__, "WM_XBUTTONUP. message=", msg.message);
            return;
        }

        if (0 != ::PeekMessage(&msg, itsWindow, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
        {
            DBG(__LINE__, "WM_KEY. message=", msg.message);
            if ((msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE))
            {
                DBG(__LINE__, "ESC Key pressed.");
                return;
            }
            // discard all other key messages
        }

        while (0 != ::PeekMessage(&msg, itsWindow, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
        {
            DBG(__LINE__, "WM_MOUSEMOVE.");
            itsMouseState.EvalMouseMsg(msg);
            if (!IsButtonDown())
            {
                DBG(__LINE__, "Button released.");
                return;
            }
            if (OutsideDragRectangle())
            {
                DBG(__LINE__, "outside drag rectangle.");
                itsDragDetected = true;
                return;
            }
            // discard mouse move
        }

    } // for
}

}


bool d1DragDetect(
    HWND hwnd,      // handle to window
    POINT init_pos, // initial position, in screen coordinates
    d1::MouseButton mb)
{
    bool res = Imp(hwnd, init_pos, mb).Result();
    return res;
}

}
