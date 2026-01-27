/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.Clipboard;


namespace WinUtil
{

namespace
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

}

auto PrivateClipFormat::instance() -> PrivateClipFormat&
{
    static Impl pc;
    return pc;
}

}
