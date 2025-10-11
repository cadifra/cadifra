/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto Combine(UndoerRef first, UndoerRef second) -> UndoerRef
{
    if (first.IsNull())
        return second;

    if (second.IsNull())
        return first;

    const bool could_merge = first.Merge(second);

    if (could_merge)
        return first;

    auto su = std::make_shared<SequenceUndoer>();

    su->Append(first);
    su->Append(second);

    return { su };
}

}
