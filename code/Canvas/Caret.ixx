/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Caret;

namespace Canvas
{

export class CaretRef;


export class ICaret
{
public:
    virtual ~ICaret() = default;

    virtual void setMaster(CaretRef& master) = 0;
    virtual void forget(CaretRef& master) = 0;
    virtual void destroy() = 0;
};


export class CaretRef
{
    ICaret* caret_; // no ownership, may be null

public:
    CaretRef(ICaret* c = 0);
    ~CaretRef(); // destroys the caret

    void destroy();

    CaretRef(const CaretRef& rhs);
    CaretRef& operator=(const CaretRef& rhs);
};


CaretRef::CaretRef(ICaret* c):
    caret_{ c }
{
    if (caret_)
        caret_->setMaster(*this);
}


CaretRef::~CaretRef()
{
    destroy();
}


void CaretRef::destroy()
{
    if (caret_)
    {
        caret_->destroy();
        caret_->forget(*this);
        caret_ = 0;
    }
}


CaretRef::CaretRef(const CaretRef& rhs):
    caret_{ rhs.caret_ }
{
    if (caret_)
        caret_->setMaster(*this);
}


CaretRef& CaretRef::operator=(const CaretRef& rhs)
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
