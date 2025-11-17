/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

void IUpdateContainer::UpdateContainer(Env& e, Param& p)
{
    if (p.Add(this))
        UpdateContainerImpl(e, p);
}

}
