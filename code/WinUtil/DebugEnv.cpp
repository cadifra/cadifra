/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Debug;


namespace WinUtil
{

namespace
{

const char* Filename = "CadifraDebug.ini";
constexpr int InitialBufSize = 128;


class Impl: public DebugEnv
{
    virtual std::string GetString(
        const std::string& section,
        const std::string& key);

    virtual int GetInt(
        const std::string& section,
        const std::string& key);

public:
    Impl() {}
};


std::string Impl::GetString(
    const std::string& section,
    const std::string& key)
{
    auto buf = std::string(InitialBufSize, '\0');

    for (;;)
    {
        DWORD res = ::GetPrivateProfileStringA(
            section.c_str(),
            key.c_str(),
            "",
            const_cast<char*>(buf.c_str()),
            static_cast<DWORD>(buf.size()),
            Filename);

        if (res == buf.size() - 1) // buffer too small
            buf.resize(buf.size() * 2);
        else
        {
            buf.resize(res);
            break;
        }
    }

    return buf;
}


int Impl::GetInt(
    const std::string& section,
    const std::string& key)
{
    return ::GetPrivateProfileIntA(
        section.c_str(), key.c_str(), 0, Filename);
}

}


DebugEnv& DebugEnv::Inst()
{
    static Impl inst;
    return inst;
}

}
