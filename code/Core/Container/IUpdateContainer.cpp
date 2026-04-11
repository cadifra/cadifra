/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core:_;
import :Container;


namespace Core
{

void IUpdateContainer::update(Env& e, Param& p)
{
    if (p.add(this))
        updateImpl(e, p);
}

}
