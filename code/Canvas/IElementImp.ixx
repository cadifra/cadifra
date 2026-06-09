/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.IElementImp;

import d1.Point;


namespace Canvas
{

export class IElementImp
{
public:
    virtual void move(const d1::fVector&) = 0;

    virtual ~IElementImp() = default;
};

}
