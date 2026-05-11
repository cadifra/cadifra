/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Canvas.Scroller;

import d1.Point;


namespace Canvas
{

namespace
{
using C = Scroller;
}


C::Scroller(const std::shared_ptr<IImp>& i):
    imp_{ i }
{
}

d1::fPoint C::autoScroll(
    const d1::Point& cursorPosition, IScrollChange* o)
{
    if (imp_)
        return imp_->autoScroll(cursorPosition, o);

    return {};
}

void C::stopAutoScroll()
{
    if (imp_)
        imp_->stopAutoScroll();
}

}
