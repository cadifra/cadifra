/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Canvas.Caret;


namespace Canvas
{

namespace
{
using C = CaretRef;
}


C::CaretRef(ICaret* c):
    caret_{ c }
{
    if (caret_)
        caret_->setMaster(*this);
}


C::~CaretRef()
{
    destroy();
}


void C::destroy()
{
    if (caret_)
    {
        caret_->destroy();
        caret_->forget(*this);
        caret_ = 0;
    }
}


C::CaretRef(const CaretRef& rhs):
    caret_{ rhs.caret_ }
{
    if (caret_)
        caret_->setMaster(*this);
}


CaretRef& C::operator=(const CaretRef& rhs)
{
    if (this == &rhs)
        return *this;

    if (caret_)
        caret_->forget(*this);

    caret_ = rhs.caret_;

    if (caret_)
        caret_->setMaster(*this);

    return *this;
}

}
