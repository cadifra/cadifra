/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

module WinUtil.ExceptionBox;

import WinUtil.UniqueHandle;

import std;


namespace WinUtil::ExceptionBox
{

namespace
{

constexpr int BuffSize = 1024;


class Buf: public std::streambuf
{
    std::vector<char_type> buf_;
    void setStreamBuf() { setp(buf_.data(), &buf_.back() - 1); }

public:
    Buf(size_t s = 1):
        buf_(s) { setStreamBuf(); }

    void reset()
    {
        memset(buf_.data(), 0, buf_.size());
        setStreamBuf();
    }

    const char_type* c_str() const { return buf_.data(); }
};


class BoxHandler
{
    Buf buffer_;
    Buf dummyBuffer_;
    std::string title_;
    UniqueHandle showEvent_;
    UniqueHandle stopEvent_;
    UniqueHandle thread_;
    static BoxHandler Singleton;

    BoxHandler();
    ~BoxHandler();
    bool showing() const;
    static DWORD WINAPI threadProc(LPVOID p);

public:
    static BoxHandler& instance() { return Singleton; }
    void setTitle(const std::string& s) { title_ = s; }
    void show();
    std::streambuf& clear();
};


BoxHandler BoxHandler::Singleton;


BoxHandler::BoxHandler():
    buffer_{ BuffSize },
    showEvent_{ CreateEvent(0, TRUE, FALSE, 0) }, // manual reset
    stopEvent_{ CreateEvent(0, TRUE, FALSE, 0) }  // manual reset
{
    thread_.reset(CreateThread(0, 0, &threadProc, this, 0, 0));
}


BoxHandler::~BoxHandler()
{
    SetEvent(stopEvent_.get());
    WaitForSingleObject(thread_.get(), INFINITE);
}


bool BoxHandler::showing() const
{
    return WAIT_TIMEOUT != WaitForSingleObject(showEvent_.get(), 0);
}


void BoxHandler::show()
{
    if (showing())
        return;
    SetEvent(showEvent_.get());
}


DWORD WINAPI BoxHandler::threadProc(LPVOID p)
{
    BoxHandler* This = reinterpret_cast<BoxHandler*>(p);

    auto icc = INITCOMMONCONTROLSEX{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES; // arbitrary value
    D1_VERIFY(InitCommonControlsEx(&icc));

    const HANDLE h[2] = { This->showEvent_.get(), This->stopEvent_.get() };
    for (;;)
    {
        if (WAIT_OBJECT_0 == WaitForMultipleObjects(2, h, FALSE, INFINITE))
        {
            MessageBoxA(0, This->buffer_.c_str(), This->title_.c_str(),
                MB_ICONHAND | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND);
            ResetEvent(h[0]);
        }
        else
            return 0;
    }
}


auto BoxHandler::clear() -> std::streambuf&
{
    Buf& b = showing() ? dummyBuffer_ : buffer_;
    b.reset();
    return b;
}


}


void setTitle(const std::string& s)
{
    BoxHandler::instance().setTitle(s);
}


auto clear() -> std::streambuf&
{
    return BoxHandler::instance().clear();
}


void show()
{
    BoxHandler::instance().show();
}

}
