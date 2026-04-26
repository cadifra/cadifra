/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core.Main;

import :UndoerImp;


namespace Core
{

auto combine(UndoerRef first, UndoerRef second) -> UndoerRef
{
    if (first.isNull())
        return second;

    if (second.isNull())
        return first;

    const bool could_merge = first.merge(second);

    if (could_merge)
        return first;

    auto su = std::make_shared<SequenceUndoer>();

    su->append(first);
    su->append(second);

    return { su };
}

}
