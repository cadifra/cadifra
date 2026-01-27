/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.ComException;

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
        HRESULT_, // dwMessageId
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

    o << getHRESULT_string() << std::ends;

    return lastWhat.c_str();
}


void C::check(d1::HRESULT r)
{
    if (FAILED(r))
        throw ComException{ r };
}


std::string C::getHRESULT_string(bool hex, bool with_prefix, bool in_parens) const
{
    auto oss = std::ostringstream{};

    if (in_parens)
        oss << "(";

    if (with_prefix)
        oss << "HRESULT=";

    if (hex)
        oss << "0x" << std::hex << HRESULT_;
    else
        oss << HRESULT_;

    if (in_parens)
        oss << ")";

    return oss.str();
}

}
