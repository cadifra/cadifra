/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

using std::ranges::for_each;

namespace
{
using C = SequenceUndoer;
}


void C::UndoImp(UndoerParam& p)
{
    auto a = [&](const auto& u) { u.Undo(p); };
    for_each(ItsUndoerList | std::views::reverse, a);
}


void C::RedoImp(UndoerParam& p)
{
    auto a = [&](const auto& u) { u.Redo(p); };
    for_each(ItsUndoerList, a);
}


void C::Append(UndoerRef u)
{
    if (!u.IsNull())
        ItsUndoerList.push_back(u);
}


bool C::IsNull() const
{
    return ItsUndoerList.empty();
}


bool C::Merge(Undoer& u)
{
    auto* s = dynamic_cast<SequenceUndoer*>(&u);

    if (!s)
        return false;

    UndoerListType non_merged;

    for (auto iu : s->ItsUndoerList)
    {
        bool merged = false;

        for (auto ju : ItsUndoerList)
        {
            if (ju.Merge(iu))
            {
                merged = true;
                break;
            }
        }

        if (!merged)
            non_merged.push_back(iu);
    }

    ItsUndoerList.insert(
        end(ItsUndoerList), begin(non_merged), end(non_merged));

    return true;
}


void C::Remove(IElement& me)
{
    for (auto i = begin(ItsUndoerList); i != end(ItsUndoerList);)
    {
        i->Remove(me);
        if (i->IsNull())
            i = ItsUndoerList.erase(i);
        else
            ++i;
    }
}

}
