/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.PrivateMessage;

namespace WinUtil
{

/*
The class PrivateMessage allows the registration of an application local
window message.

With this class, you do not need to maintain a global header file that contains
all user defined messages which creates unwanted dependencies between unrelated
packages.

PrivateMessage uses message numbers in the range (WM_USER+0x200)...(WM_APP-1).

Note the difference to the operating system function RegisterWindowMessage,
which creates message numbers that are unique among all applications.
*/
export class PrivateMessage
{
public:
    static PrivateMessage& instance();

    virtual unsigned int getNumber() = 0;
    // returns a message number that is unique within the application

protected:
    ~PrivateMessage() = default;
};

}


module : private;


namespace WinUtil
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


auto PrivateMessage::instance() -> PrivateMessage&
{
    static Impl pm;
    return pm;
}

}
