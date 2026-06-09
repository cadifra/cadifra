/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.SequenceUndoer;

import Core.Main;

import std;


namespace Core
{

export class SequenceUndoer: public Undoer
{
public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool isNull() const override { return undoerList_.empty(); }
    bool merge(Undoer& u) override;
    void remove(IElement&) override;

    //--

    SequenceUndoer() {}
    SequenceUndoer(const SequenceUndoer&) = delete;
    SequenceUndoer& operator=(const SequenceUndoer& rhs) = delete;

    void append(UndoerRef);
    // Append the Undoer to this SequenceUndoer.

private:
    using UndoerListType = std::vector<UndoerRef>;
    UndoerListType undoerList_;
};


// Creates an single Undoer from a ordered pair of Undoers.
// In contrast to Under::merge(), combine() is always successful.
// first and second can be any Undoer, including NullUndoers.
//
// PRE: first and second must have been created from
// consecutive transactions.
//
export auto combine(UndoerRef first, UndoerRef second) -> UndoerRef;

}

module :private;


namespace Core
{

using std::ranges::for_each;


void SequenceUndoer::undoImp(Param& p)
{
    auto a = [&](const auto& u) { u.undo(p); };
    for_each(undoerList_ | std::views::reverse, a);
}


void SequenceUndoer::redoImp(Param& p)
{
    auto a = [&](const auto& u) { u.redo(p); };
    for_each(undoerList_, a);
}


void SequenceUndoer::append(UndoerRef u)
{
    if (not u.isNull())
        undoerList_.push_back(u);
}


bool SequenceUndoer::merge(Undoer& u)
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


void SequenceUndoer::remove(IElement& me)
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
