/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

module WinUtil;

import std;


namespace WinUtil::ExceptionBox
{

namespace
{

constexpr int BuffSize = 1024;


class Buf: public std::streambuf
{
    std::vector<char_type> itsBuf;
    void SetStreamBuf() { setp(itsBuf.data(), &itsBuf.back() - 1); }

public:
    Buf(size_t s = 1):
        itsBuf(s) { SetStreamBuf(); }

    void Reset()
    {
        memset(itsBuf.data(), 0, itsBuf.size());
        SetStreamBuf();
    }

    const char_type* c_str() const { return itsBuf.data(); }
};


class BoxHandler
{
    Buf itsBuffer;
    Buf itsDummyBuffer;
    std::string itsTitle;
    WinUtil::UniqueHandle itsShowEvent;
    WinUtil::UniqueHandle itsStopEvent;
    WinUtil::UniqueHandle itsThread;
    static BoxHandler Singleton;

    BoxHandler();
    ~BoxHandler();
    bool Showing() const;
    static DWORD WINAPI ThreadProc(LPVOID p);

public:
    static BoxHandler& Instance() { return Singleton; }
    void SetTitle(const std::string& s) { itsTitle = s; }
    void Show();
    std::streambuf& Clear();
};


BoxHandler BoxHandler::Singleton;


BoxHandler::BoxHandler():
    itsBuffer{ BuffSize },
    itsShowEvent{ CreateEvent(0, TRUE, FALSE, 0) }, // manual reset
    itsStopEvent{ CreateEvent(0, TRUE, FALSE, 0) }  // manual reset
{
    itsThread.reset(CreateThread(0, 0, &ThreadProc, this, 0, 0));
}


BoxHandler::~BoxHandler()
{
    SetEvent(itsStopEvent.get());
    WaitForSingleObject(itsThread.get(), INFINITE);
}


bool BoxHandler::Showing() const
{
    return WAIT_TIMEOUT != WaitForSingleObject(itsShowEvent.get(), 0);
}


void BoxHandler::Show()
{
    if (Showing())
        return;
    SetEvent(itsShowEvent.get());
}


DWORD WINAPI BoxHandler::ThreadProc(LPVOID p)
{
    BoxHandler* This = reinterpret_cast<BoxHandler*>(p);

    auto icc = INITCOMMONCONTROLSEX{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES; // arbitrary value
    D1_VERIFY(InitCommonControlsEx(&icc));

    const HANDLE h[2] = { This->itsShowEvent.get(), This->itsStopEvent.get() };
    for (;;)
    {
        if (WAIT_OBJECT_0 == WaitForMultipleObjects(2, h, FALSE, INFINITE))
        {
            MessageBoxA(0, This->itsBuffer.c_str(), This->itsTitle.c_str(),
                MB_ICONHAND | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND);
            ResetEvent(h[0]);
        }
        else
            return 0;
    }
}


auto BoxHandler::Clear() -> std::streambuf&
{
    Buf& b = Showing() ? itsDummyBuffer : itsBuffer;
    b.Reset();
    return b;
}


}


void SetTitle(const std::string& s)
{
    BoxHandler::Instance().SetTitle(s);
}


auto Clear() -> std::streambuf&
{
    return BoxHandler::Instance().Clear();
}


void Show()
{
    BoxHandler::Instance().Show();
}

}
