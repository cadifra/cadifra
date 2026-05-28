/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.PrivateClipFormat;


namespace WinUtil
{

/*
The class PrivateClipFormat allows the registration of an application local
clipboard format.

With this class, you do not need to maintain a global header file that contains
all private clipboard formats which creates unwanted dependencies between
unrelated packages.

PrivateClipFormat uses clipboard format numbers in the range
CF_PRIVATEFIRST...CF_PRIVATELAST.

Note the difference to the operating system function RegisterClipboardFormat,
which creates clipboard format numbers that are unique among all applications.
*/
export class PrivateClipFormat
{
public:
    static auto instance() -> PrivateClipFormat&;

    virtual CLIPFORMAT getCLIPFORMAT() = 0;
    // returns a clipboard format number that is unique
    // within the application

protected:
    ~PrivateClipFormat() = default;
};

}


module : private;


namespace WinUtil
{

class Impl: public PrivateClipFormat
{
public:
    Impl();
    CLIPFORMAT getCLIPFORMAT() final;

private:
    CLIPFORMAT nextClipFormat_;
};


Impl::Impl():
    nextClipFormat_{ CF_PRIVATEFIRST }
{
}


CLIPFORMAT Impl::getCLIPFORMAT()
{
    D1_ASSERT(nextClipFormat_ < CF_PRIVATELAST);
    return nextClipFormat_++;
}


auto PrivateClipFormat::instance() -> PrivateClipFormat&
{
    static Impl pc;
    return pc;
}

}
