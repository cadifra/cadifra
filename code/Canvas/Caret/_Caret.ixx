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

}
