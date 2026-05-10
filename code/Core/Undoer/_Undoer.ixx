/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Undoer;

import Core.Main;

import d1.Point;

namespace Core
{

export class PosUndoerGroup: public Undoer
{
    using V = std::vector<std::shared_ptr<IPosOwner>>;

    V objects_;
    const d1::Vector offset_;

public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool merge(Undoer& u) override;
    void remove(IElement&) override;
    bool isNull() const override;

    //--

    PosUndoerGroup(const d1::Vector& offset);

    PosUndoerGroup(const PosUndoerGroup&) = delete;
    PosUndoerGroup& operator=(const PosUndoerGroup&) = delete;

    void add(IPosOwner&);

    const d1::Vector& offset() const { return offset_; }
};


export class SequenceUndoer: public Undoer
{
    using UndoerListType = std::vector<UndoerRef>;
    UndoerListType undoerList_;

    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool isNull() const override;
    bool merge(Undoer& u) override;
    void remove(IElement&) override;

    //--

public:
    SequenceUndoer() {}
    SequenceUndoer(const SequenceUndoer&) = delete;
    SequenceUndoer& operator=(const SequenceUndoer& rhs) = delete;

public:
    void append(UndoerRef);
    // Append the Undoer to this SequenceUndoer.
};


export class TransactionUndoer: public Undoer
{
    using MESet = std::vector<IElementRef>;

    UndoerRef touchedUndoers_;
    MESet newlyCreatedElements_;
    ElementSet trashedElements_;
    MESet untrashedClients_;
    ElementSet uncreatedClients_;
    d1::uint32 transactionNo_;

public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool isNull() const override;
    bool merge(Undoer& u) override;
    void remove(IElement&) override;

    //--

    TransactionUndoer(
        UndoerRef theTouchedUndoer,
        const MESet& theNewlyCreatedElements,
        ElementSet theTrashedElements,
        d1::uint32 theTransactionNo);

private:
    static void findCommonElements(MESet& res, const MESet&, const ElementSet&);
};


// Creates an single Undoer from a ordered pair of Undoers.
// In contrast to Under::merge(), combine() is always successful.
// first and second can be any Undoer, including NullUndoers.
//
// PRE: first and second must have been created from
// consecutive transactions.
//
export auto combine(UndoerRef first, UndoerRef second) -> UndoerRef
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
