/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <conio.h>
#include <tchar.h>

#include <Windows.h>

module WinUtil;

import std;


namespace WinUtil
{

namespace
{

constexpr int BuffSize = 1024;


bool GetDebugEnv(const std::string& s)
{
    return DebugEnv::Inst().GetInt("dout", s) != 0;
}

bool ToConsole() { return GetDebugEnv("WriteToConsole"); }
bool ToConsoleAndDebugger() { return GetDebugEnv("WriteToConsoleAndDebugger"); }
bool CloseConsoleOnExit() { return GetDebugEnv("CloseConsoleOnExit"); }

}


class DebugOstream::Buf:
    public std::basic_streambuf<DebugOstream::char_type,
        std::char_traits<DebugOstream::char_type>>
{
public:
    Buf();
    ~Buf() final;

protected:
    int_type overflow(int_type c) final;
    int sync() final;

private:
    using Buffer = std::basic_string<DebugOstream::char_type>;
    Buffer itsBuf;
    const bool itWritesToConsole;
    const bool itWritesToDebugger;
};


DebugOstream::Buf::Buf():
    itsBuf(BuffSize, 0),
    itWritesToConsole{ ToConsole() || ToConsoleAndDebugger() },
    itWritesToDebugger{ !ToConsole() || ToConsoleAndDebugger() }
{
    if (itWritesToConsole)
        ::AllocConsole();
    setp(&*std::begin(itsBuf), &*(std::end(itsBuf) - 1));
}


DebugOstream::Buf::~Buf()
{
    sync();

    if (itWritesToConsole && !CloseConsoleOnExit())
    {
        auto s = STARTUPINFO{};
        s.cb = sizeof(s);
        auto p = PROCESS_INFORMATION{};
        TCHAR commandLine[128];
        _tcscpy(commandLine, TEXT("cmd.exe"));
        // hand over our console to a new (dummy) process:
        ::CreateProcess(0, commandLine, 0, 0, FALSE, 0, 0, 0, &s, &p);
    }
}


auto DebugOstream::Buf::overflow(Buf::int_type c) -> int_type
{
    if (c != Buf::traits_type::eof())
    {
        *pptr() = c;

        if (itWritesToConsole)
            ::_cputs(itsBuf.c_str());

        if (itWritesToDebugger)
            ::OutputDebugStringA(itsBuf.c_str());

        pbump(-static_cast<int>(pptr() - pbase()));
    }
    return std::char_traits<char_type>::not_eof(c);
}


int DebugOstream::Buf::sync()
{
    *pptr() = 0;

    if (itWritesToConsole)
        ::_cputs(itsBuf.c_str());

    if (itWritesToDebugger)
        ::OutputDebugStringA(itsBuf.c_str());

    pbump(-static_cast<int>(pptr() - pbase()));

    return 0;
}


auto dout = DebugOstream{ std::make_unique<DebugOstream::Buf>() };


DebugOstream::DebugOstream(std::unique_ptr<Buf> buf):
    inherited{ buf.get() },
    itsBuf{ std::move(buf) }
{
}

DebugOstream::~DebugOstream() = default;


}
