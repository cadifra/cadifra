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

    unsigned int getNumber() final;

private:
    unsigned int nextNumber_;
};

Impl::Impl():
    nextNumber_{ First }
{
}

unsigned int Impl::getNumber()
{
    D1_ASSERT(nextNumber_ < Last);
    return nextNumber_++;
}

}

auto PrivateMessage::instance() -> PrivateMessage&
{
    static Impl pm;
    return pm;
}

}
