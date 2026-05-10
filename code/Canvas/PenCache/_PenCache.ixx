/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

export module Canvas.PenCache;

import Canvas.GdiObj;

import WinUtil.types;

import std;


namespace Canvas
{

export namespace PenCache
{
auto Default() -> GdiObj<WinUtil::HPEN>;
auto marker() -> GdiObj<WinUtil::HPEN>;
auto blackDashed() -> GdiObj<WinUtil::HPEN>;
auto alternate() -> GdiObj<WinUtil::HPEN>;
auto null() -> GdiObj<WinUtil::HPEN>;
}

}
