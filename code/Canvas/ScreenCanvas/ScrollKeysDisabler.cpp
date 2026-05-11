/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Canvas.ScreenCanvas;


namespace Canvas
{

namespace
{
using C = ScrollKeysDisabler;
}


C::ScrollKeysDisabler(IScreenCanvas& sc):
    screenCanvas_{ sc }
{
    screenCanvas_.disableScrollKeys();
}


C::~ScrollKeysDisabler()
{
    screenCanvas_.enableScrollKeys();
}

}
