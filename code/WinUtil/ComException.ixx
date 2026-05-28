/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.ComException;

import d1.Exception;
import d1.wintypes;

import std;


namespace WinUtil
{

export class ComException: public d1::Exception
{
public:
    //-- std::exception

    const char* what() const override;
    // The returned pointer is only valid until the next
    // call of ComException::what

    //--

    static void check(d1::HRESULT r);

    ComException(d1::HRESULT r):
        HRESULT_{ r }
    {
    }

    d1::HRESULT getHRESULT() const { return HRESULT_; }

    std::string getHRESULT_string(
        bool hex = true, bool with_prefix = true, bool in_parens = true) const;

    // uses compiler generated copy ctor and assignment operator

private:
    d1::HRESULT HRESULT_;

    static std::string lastWhat;
    static std::string lastMessage;
};

}

module : private;


namespace WinUtil
{

constexpr int MAX_MESSAGE_SIZE = 2048;


auto ComException::lastMessage = std::string(MAX_MESSAGE_SIZE, '\0');
auto ComException::lastWhat = std::string(MAX_MESSAGE_SIZE + 256, '\0');


const char* ComException::what() const
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


void ComException::check(d1::HRESULT r)
{
    if (FAILED(r))
        throw ComException{ r };
}


std::string ComException::getHRESULT_string(
    bool hex, bool with_prefix, bool in_parens) const
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
