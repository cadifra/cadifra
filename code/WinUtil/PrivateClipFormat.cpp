/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil;


namespace WinUtil
{

namespace
{

class Impl: public PrivateClipFormat
{
public:
    Impl();
    CLIPFORMAT Register() final;

private:
    CLIPFORMAT itsNextClipFormat;
};

Impl::Impl():
    itsNextClipFormat{ CF_PRIVATEFIRST }
{
}

CLIPFORMAT Impl::Register()
{
    D1_ASSERT(itsNextClipFormat < CF_PRIVATELAST);
    return itsNextClipFormat++;
}

}

auto PrivateClipFormat::Instance() -> PrivateClipFormat&
{
    static Impl pc;
    return pc;
}

}
