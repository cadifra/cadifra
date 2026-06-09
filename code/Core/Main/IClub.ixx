/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IClub;

import d1.Iterator;


namespace Core
{

export
{
class IElement;
}

// An IClub contains a set of model elements that may be visible
// in a view. A club is closed, this means that all references
// to elements of all elements in the club refer only to elements
// inside the club.
//
export class IClub
{
public:
    virtual auto begin() const -> d1::Iterator<IElement*> = 0;
    virtual auto end() const -> d1::Iterator<IElement*> = 0;

    friend auto begin(const IClub& c)
    {
        return c.begin();
    }

    friend auto end(const IClub& c)
    {
        return c.end();
    }

protected:
    ~IClub() = default;
};


}
