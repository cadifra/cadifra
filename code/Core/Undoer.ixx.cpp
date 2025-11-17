/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core:Undoer;

import :Base;


namespace Core
{

class PosUndoerGroup: public Undoer
{
    using V = std::vector<std::shared_ptr<IPosOwner>>;

    V itsObjects;
    const d1::Vector itsOffset;

public:
    //-- Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;
    bool IsNull() const override;

    //--

    PosUndoerGroup(const d1::Vector& offset);

    PosUndoerGroup(const PosUndoerGroup&) = delete;
    PosUndoerGroup& operator=(const PosUndoerGroup&) = delete;

    void Add(IPosOwner&);

    const d1::Vector& Offset() const { return itsOffset; }
};


class SequenceUndoer: public Undoer
{
    using UndoerListType = std::vector<UndoerRef>;
    UndoerListType ItsUndoerList;

    //-- Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool IsNull() const override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;

    //--

public:
    SequenceUndoer() {}
    SequenceUndoer(const SequenceUndoer&) = delete;
    SequenceUndoer& operator=(const SequenceUndoer& rhs) = delete;

public:
    void Append(UndoerRef);
    // Append the Undoer to this SequenceUndoer.
};


class TransactionUndoer: public Undoer
{
    using MESet = std::vector<IElementRef>;

    UndoerRef itsTouchedUndoers;
    MESet itsNewlyCreatedElements;
    ElementSet itsTrashedElements;
    MESet itsUntrashedClients;
    ElementSet itsUncreatedClients;
    d1::uint32 itsTransactionNo;

public:
    //-- Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool IsNull() const override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;

    //--

    TransactionUndoer(
        UndoerRef theTouchedUndoer,
        const MESet& theNewlyCreatedElements,
        ElementSet theTrashedElements,
        d1::uint32 theTransactionNo);

private:
    static void FindCommonElements(MESet& res, const MESet&, const ElementSet&);
};

}
