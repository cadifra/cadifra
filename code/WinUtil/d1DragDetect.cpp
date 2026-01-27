/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include <Windowsx.h>

#include "d1/d1verify.h"

#include <stdlib.h> // abs

module WinUtil.d1DragDetect;

import WinUtil.Debug;

import d1.ScopeGuard;

import std;


#ifdef _DEBUG
#define D1_DRAG_DETECT_TEST 0 // set to 1 for testing
#endif


namespace WinUtil
{

namespace
{


#ifdef _DEBUG
bool debugEnabled()
{
    return (WinUtil::DebugEnv::inst().getInt(
                "WinUtil", "d1DragDetect") != 0);
}
#endif


#if defined(_DEBUG) and D1_DRAG_DETECT_TEST
constexpr LONGLONG DRAG_DETECT_TIMEOUT = -4000 * 10000;
#else
constexpr LONGLONG DRAG_DETECT_TIMEOUT = -500 * 10000; // time in 100ns, negative means relative
#endif


enum class UpDown
{
    up = 1,
    down
};

UINT buttonMessageCode(const d1::MouseButton mb, const UpDown ud)
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

    void evalMouseMsg(const MSG& msg)
    {
        leftButtonDown = (msg.wParam & MK_LBUTTON) != 0;
        middleButtonDown = (msg.wParam & MK_MBUTTON) != 0;
        rightButtonDown = (msg.wParam & MK_RBUTTON) != 0;
        pos.x = GET_X_LPARAM(msg.lParam);
        pos.y = GET_Y_LPARAM(msg.lParam);
    }

    bool isButtonDown(d1::MouseButton mb) const
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
    const int saved_SM_CXDRAG_;
    const int saved_SM_CYDRAG_;
    int factor_;

public:
    TestEnlargeDragRect(int factor = 8):
        saved_SM_CXDRAG_{ ::GetSystemMetrics(SM_CXDRAG) },
        saved_SM_CYDRAG_{ ::GetSystemMetrics(SM_CYDRAG) },
        factor_{ 1 }
    {
        enlargeDragRect(factor);
    }
    ~TestEnlargeDragRect()
    {
        restoreInitialDragRect();
    }
    int getFactor() const { return factor_; }

private:
    static void setDragRect(int width, int height)
    {
        D1_VERIFY(0 != ::SystemParametersInfo(SPI_SETDRAGWIDTH, width, 0, 0));
        D1_VERIFY(0 != ::SystemParametersInfo(SPI_SETDRAGHEIGHT, height, 0, 0));
    }
    void restoreInitialDragRect() const
    {
        setDragRect(saved_SM_CXDRAG_, saved_SM_CYDRAG_);
    }
    void enlargeDragRect(int factor)
    {
        setDragRect(
            factor * ::GetSystemMetrics(SM_CXDRAG),
            factor * ::GetSystemMetrics(SM_CYDRAG));
        factor_ *= factor;
    }
};

#endif


class Imp
{
    const HWND window_ = {};
    const POINT initPosScreen_ = {};
    const POINT initPos_ = {}; // in client coordinates
    const d1::MouseButton button_;

    MouseState mouseState_;

    bool dragDetected_ = false;

#ifdef _DEBUG
    const bool debugEnabled_{ debugEnabled() };
#if D1_DRAG_DETECT_TEST
    TestEnlargeDragRect testEnlargeDragRect_;
#endif
#endif

    const int SM_CXDRAG_;
    const int SM_CYDRAG_;

public:
    Imp(
        HWND hwnd,      // handle to window
        POINT init_pos, // initial position in screen coordinates
        d1::MouseButton mb);

    ~Imp();

    bool result() const { return dragDetected_; }

    POINT getMousePos() const { return mouseState_.pos; }

    bool outsideDragRectangle(const POINT& p) const;
    // returns true, if p is outside drag rectangle defined by system metrics.
    // p is in client coordinates.

    bool outsideDragRectangle() const
    {
        return outsideDragRectangle(getMousePos());
    }

private:
    void work();

    bool isButtonDown() const
    {
        return mouseState_.isButtonDown(button_);
    }

    UINT buttonMsgCode(UpDown ud) const
    {
        return buttonMessageCode(button_, ud);
    }

#ifdef _DEBUG

    template <class T>
    void DBG(int line, const char* text, const T& t) const
    {
        if (debugEnabled_)
            WinUtil::dout
                << "@@@ WinUtil::d1DragDetect #" << line
                << ": " << text << t << std::endl;
    }

    void DBG(int line, const char* text) const
    {
        if (debugEnabled_)
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


bool doPostMessage(HWND hWnd, const MSG& msg)
{
    return ::PostMessage(hWnd, msg.message, msg.wParam, msg.lParam) != 0;
}


POINT toClient(HWND hwnd, POINT pos)
{
    D1_VERIFY(::ScreenToClient(hwnd, &pos));
    return pos;
}


Imp::Imp(
    HWND hwnd, // handle to window
    POINT init_pos,
    d1::MouseButton mb):

    window_{ hwnd },
    initPosScreen_{ init_pos },
    initPos_{ toClient(hwnd, init_pos) },
    button_{ mb },
    SM_CXDRAG_{ ::GetSystemMetrics(SM_CXDRAG) },
    SM_CYDRAG_{ ::GetSystemMetrics(SM_CYDRAG) }
{
    ::SetCapture(window_);
    work();
}


Imp::~Imp()
{
    D1_VERIFY(::ReleaseCapture() != 0);

    if (dragDetected_)
        DBG(__LINE__, "### DRAG DETECTED ###");
    else
        DBG(__LINE__, "--- finished. no drag ---");
}


bool Imp::outsideDragRectangle(const POINT& p) const
{
    if (SM_CXDRAG_ < 2 * abs(p.x - initPos_.x))
        return true;

    if (SM_CYDRAG_ < 2 * abs(p.y - initPos_.y))
        return true;

    return false;
}


void Imp::work()
{
    DBG(__LINE__, "started. Button=", (int)button_);
    DBG(__LINE__, "SM_CXDRAG=", SM_CXDRAG_);
    DBG(__LINE__, "SM_CYDRAG=", SM_CYDRAG_);
    if (not window_)
    {
        DBG(__LINE__, "not window. returning false.");
        return;
    }

    if (button_ == d1::MouseButton::left)
    {
        DBG(__LINE__, "calling API DragDetect");
        dragDetected_ = (::DragDetect(window_, initPosScreen_) != 0);
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
            dragDetected_ = true;
            return;
        }

        auto msg = MSG{};

        const UINT up_msg_id = buttonMsgCode(UpDown::up);
        if (0 != ::PeekMessage(&msg, window_, up_msg_id, up_msg_id, PM_NOREMOVE))
        {
            DBG(__LINE__, "WM_XBUTTONUP. message=", msg.message);
            return;
        }

        if (0 != ::PeekMessage(&msg, window_, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
        {
            DBG(__LINE__, "WM_KEY. message=", msg.message);
            if ((msg.message == WM_KEYDOWN) and (msg.wParam == VK_ESCAPE))
            {
                DBG(__LINE__, "ESC Key pressed.");
                return;
            }
            // discard all other key messages
        }

        while (0 != ::PeekMessage(&msg, window_, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
        {
            DBG(__LINE__, "WM_MOUSEMOVE.");
            mouseState_.evalMouseMsg(msg);
            if (not isButtonDown())
            {
                DBG(__LINE__, "Button released.");
                return;
            }
            if (outsideDragRectangle())
            {
                DBG(__LINE__, "outside drag rectangle.");
                dragDetected_ = true;
                return;
            }
            // discard mouse move
        }

    } // for
}

}


bool d1DragDetect(
    d1::HWND hwnd,      // handle to window
    d1::POINT init_pos, // initial position, in screen coordinates
    d1::MouseButton mb)
{
    bool res = Imp(hwnd, init_pos, mb).result();
    return res;
}

}
