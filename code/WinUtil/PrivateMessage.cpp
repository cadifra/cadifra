/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.MessageLoop;


namespace WinUtil
{

namespace
{
constexpr unsigned int
    First = WM_USER + 0x200,
    Last = WM_APP - 1;

class Impl: public PrivateMessage
{
public:
    Impl();

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl& rhs) = delete;

    unsigned int Register() final;

private:
    unsigned int itsNextNumber;
};

Impl::Impl():
    itsNextNumber{ First }
{
}

unsigned int Impl::Register()
{
    D1_ASSERT(itsNextNumber < Last);
    return itsNextNumber++;
}

}

auto PrivateMessage::Instance() -> PrivateMessage&
{
    static Impl pm;
    return pm;
}

}
