/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;
import :UndoerImp;


namespace Core
{

using std::ranges::for_each;

namespace
{
using C = SequenceUndoer;
}


void C::undoImp(Param& p)
{
    auto a = [&](const auto& u) { u.undo(p); };
    for_each(undoerList_ | std::views::reverse, a);
}


void C::redoImp(Param& p)
{
    auto a = [&](const auto& u) { u.redo(p); };
    for_each(undoerList_, a);
}


void C::append(UndoerRef u)
{
    if (not u.isNull())
        undoerList_.push_back(u);
}


bool C::isNull() const
{
    return undoerList_.empty();
}


bool C::merge(Undoer& u)
{
    auto* s = dynamic_cast<SequenceUndoer*>(&u);

    if (not s)
        return false;

    UndoerListType non_merged;

    for (auto iu : s->undoerList_)
    {
        bool merged = false;

        for (auto ju : undoerList_)
        {
            if (ju.merge(iu))
            {
                merged = true;
                break;
            }
        }

        if (not merged)
            non_merged.push_back(iu);
    }

    undoerList_.insert(
        end(undoerList_), cbegin(non_merged), cend(non_merged));

    return true;
}


void C::remove(IElement& me)
{
    for (auto i = begin(undoerList_); i != end(undoerList_);)
    {
        i->remove(me);
        if (i->isNull())
            i = undoerList_.erase(i);
        else
            ++i;
    }
}

}
