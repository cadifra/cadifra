/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Main:Undoer;

import :IElement;
import :IDiagram;


namespace Core
{

export {
class PosUndoer;
}


export class Undoer // note: use UndoerRef wherever possible (see below)
{
public:
    virtual ~Undoer() = default;

    virtual bool isNull() const { return false; }
    // returns true, if this Undoer is a NullUndoer.
    // A NullUndoer does not do anything, if it's Undo or Redo member functions
    // are called. To create a NullUndoer, use UndoerRef::makeNullUndoer().

    class Param;

    void undo(Param&);
    void redo(Param&);

private:
    virtual void undoImp(Param&) = 0;
    virtual void redoImp(Param&) = 0;

public:
    virtual bool merge(Undoer& u) = 0;
    // If possible, Merge retrieves all undo information from u, merges it
    // into this Undoer and returns true.
    // If merging is impossible, Merge makes nothing and returns false.
    // If Merge returns true, the caller may safely delete u without loosing
    // any information.
    //
    // Precondition: This Undoer is older than u
    //
    // Example:
    // You have to Undoers a (older) and b (younger).
    // The following two sequences are identical if Merge returns true:
    //
    //          a.undo(...)          a.merge(b)
    //          b.undo(...)          a.undo(...)
    //          b.redo(...)          a.redo(...)
    //          a.redo(...)

    virtual PosUndoer* getPosUndoer() { return 0; }

    virtual void remove(IElement&) = 0;

protected:
    Undoer()
    {
    }

    Undoer(const Undoer&) = delete;
    Undoer& operator=(const Undoer& rhs) = delete;
};


class Undoer::Param
{
public:
    Param(IDiagram& d, Selection::Tracker& sc);

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    ~Param(); // calls Finish

    void finish(); // may be called several times

    void addToDiagram(const IElementRef&);
    void removeFromDiagram(const IElementRef&);
    void updateViews(const IElementRef&);

    auto diagram() -> IDiagram& { return diagram_; }

private:
    using MESet = std::set<IElementRef>;

    MESet addToDiagram_;
    MESet removeFromDiagram_;
    MESet updateViews_;

    IDiagram& diagram_;
    Selection::Tracker& selectionTracker_; // to be removed
};


export class UndoerRef
{
    std::shared_ptr<Undoer> undoer_;

public:
    UndoerRef()
    {
        UndoerRef null = makeNullUndoer();
        undoer_ = null.undoer_;
    }

    UndoerRef(const std::shared_ptr<Undoer>& rhs):
        undoer_{ rhs }
    {
    }

    UndoerRef(const UndoerRef& rhs) = default;
    UndoerRef& operator=(const UndoerRef& rhs) = default;

    bool isNull() const
    {
        return undoer_->isNull();
    }

    static auto makeNullUndoer() -> UndoerRef;

    void undo(Undoer::Param& p) const
    {
        undoer_->undo(p);
    }

    void redo(Undoer::Param& p) const
    {
        undoer_->redo(p);
    }

    auto get() const -> Undoer* { return undoer_.get(); }

    bool merge(UndoerRef u)
    {
        return undoer_->merge(*(u.undoer_.get()));
    }
    // If possible, Merge retrieves all undo information from the Undoer referenced
    // by u, merges it into the Undoer referenced by this UndoerRef and returns
    // true.
    // If a merge is impossible, Merge has no effect and returns false.

    void remove(IElement& me)
    {
        undoer_->remove(me);
    }
};

}

namespace Core
{

auto UndoerRef::makeNullUndoer() -> UndoerRef
{
    class NullUndoer: public Undoer
    {
    public:
        virtual bool isNull() const final { return true; }
        virtual void undoImp(Param&) final {}
        virtual void redoImp(Param&) final {}
        virtual bool merge(Undoer& u) final { return u.isNull(); }
        virtual void remove(IElement&) final {}
    };

    static std::shared_ptr<Undoer> res = std::make_shared<NullUndoer>();
    return res;
}


namespace
{
class UndoRedoReporter
{
    Undoer& u_;
    IDiagram& d_;

public:
    UndoRedoReporter(Undoer& u, IDiagram& d):
        u_{ u },
        d_{ d }
    {
        d_.beginUndoRedo(u_);
    }
    ~UndoRedoReporter()
    {
        d_.endUndoRedo(u_);
    }
};
}


void Undoer::undo(Param& p)
{
    auto r = UndoRedoReporter{ *this, p.diagram() };
    undoImp(p);
}


void Undoer::redo(Param& p)
{
    auto r = UndoRedoReporter{ *this, p.diagram() };
    redoImp(p);
}


Undoer::Param::Param(IDiagram& d, Selection::Tracker& sc):
    diagram_{ d },
    selectionTracker_{ sc }
{
}


Undoer::Param::~Param()
{
    finish();
}


void Undoer::Param::finish()
{
    for (auto me : addToDiagram_)
    {
        D1_ASSERT(not me->isInTrash());
        diagram_.insert(me);
        diagram_.createViewElements(*me);
    }
    addToDiagram_.clear();

    for (auto mi : removeFromDiagram_)
    {
        IElement& me = *mi;
        D1_ASSERT(me.isInTrash());
        diagram_.remove(me);
        diagram_.destroyViewElements(me, selectionTracker_);
    }
    removeFromDiagram_.clear();

    for (auto mi : updateViews_)
    {
        IElement& me = *mi;
        if (me.isInTrash())
            continue;
        me.viewsNeedUpdate(diagram_);
    }
    updateViews_.clear();
}


void Undoer::Param::addToDiagram(const IElementRef& me)
{
    addToDiagram_.insert(me);
}


void Undoer::Param::removeFromDiagram(const IElementRef& me)
{
    removeFromDiagram_.insert(me);
}


void Undoer::Param::updateViews(const IElementRef& me)
{
    updateViews_.insert(me);
}

}
