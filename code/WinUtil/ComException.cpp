/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;

import std;


namespace WinUtil
{

namespace
{
constexpr int MAX_MESSAGE_SIZE = 2048;

using C = ComException;
}

auto C::lastMessage = std::string(MAX_MESSAGE_SIZE, '\0');
auto C::lastWhat = std::string(MAX_MESSAGE_SIZE + 256, '\0');


const char* C::what() const
{
    const DWORD res = ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        0,          // lpSource
        itsHRESULT, // dwMessageId
        0,          // dwLanguageId
        const_cast<char*>(lastMessage.c_str()),
        static_cast<DWORD>(lastMessage.capacity()),
        0 // Arguments
    );

    auto o = std::ostringstream{ lastWhat };

    if (res)
    {
        o << lastMessage.c_str();
    }

    o << GetHRESULT_string() << std::ends;

    return lastWhat.c_str();
}


std::string C::GetHRESULT_string(bool hex, bool with_prefix, bool in_parens) const
{
    auto oss = std::ostringstream{};

    if (in_parens)
        oss << "(";

    if (with_prefix)
        oss << "HRESULT=";

    if (hex)
        oss << "0x" << std::hex << itsHRESULT;
    else
        oss << itsHRESULT;

    if (in_parens)
        oss << ")";

    return oss.str();
}

}
