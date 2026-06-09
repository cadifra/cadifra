/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Group;

import d1.Iterator;


namespace Core
{

export
{
class IElement;
}


export class Group
{
public:
    virtual void assignIDs() = 0;

    virtual auto begin() const -> d1::Iterator<IElement*> = 0;
    virtual auto end() const -> d1::Iterator<IElement*> = 0;

    Group() = default;
    virtual ~Group() = default;

    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    friend auto begin(const Group& g)
    {
        return g.begin();
    }

    friend auto end(const Group& g)
    {
        return g.end();
    }
};

}
