/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Undoer;

import :Element;


namespace Core
{

export class PosUndoer;


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
    using MESet = std::set<IElementRef>;

    MESet addToDiagram_;
    MESet removeFromDiagram_;
    MESet updateViews_;

    IDiagram& diagram_;
    Selection::Tracker& selectionTracker_; // to be removed

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
};


export class IPosOwner: public virtual IElement
{
public:
    virtual void move(const d1::Vector& offset) = 0;
};


export class PosUndoer: public Undoer
{
    std::shared_ptr<IPosOwner> object_;
    const d1::Vector offset_;

public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool merge(Undoer& u) override;
    PosUndoer* getPosUndoer() override { return this; }
    void remove(IElement&) override;
    bool isNull() const override;

    //--

    PosUndoer(IPosOwner& theObject, const d1::Vector& offset);

    PosUndoer(const PosUndoer&) = delete;
    PosUndoer& operator=(const PosUndoer&) = delete;

    IPosOwner& object() const { return *object_; }
    const d1::Vector& offset() const { return offset_; }
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


export auto combine(UndoerRef first, UndoerRef second) -> UndoerRef;
//
// Creates an single Undoer from a ordered pair of Undoers.
// In contrast to Under::merge(), combine() is always successful.
// first and second can be any Undoer, including NullUndoers.
//
// PRE: first and second must have been created from
// consecutive transactions.

}
