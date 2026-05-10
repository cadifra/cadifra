/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.TransferSet;

import Core.Main;
import Core.ObjectID;

import std;


namespace Core
{

export class TransferSet: public IClub
{
public:
    class Copier;

    using MeSet = std::vector<IElementRef>;

private:
    MeSet elements_;
    IElement* focus_ = nullptr; // one of elements_. ref only
    Copier* copier_ = nullptr;  // ref only. May be zero

    std::unique_ptr<Group> group_;

public:
    //-- Core::IClub

    auto begin() const -> d1::Iterator<IElement*> override;
    auto end() const -> d1::Iterator<IElement*> override;

    //--

    TransferSet() = default;

    TransferSet(const TransferSet&) = delete;
    TransferSet& operator=(const TransferSet&) = delete;

    virtual ~TransferSet();

    void add(IElementRef);
    void remove(IElement&);
    void setFocus(ObjectID);

    struct PasteRes
    {
        IElement* focus = nullptr; // ref only, may be zero
        ElementSet pasted_elements;
        PasteRes() {}
    };

    auto paste(Transaction& t) const -> PasteRes;
    // Pastes the contents of the TransferSet into the diagram defined
    // through the active Transaction t.
    // The elements are pasted at the same position from where
    // they were copied.
    // Returns a pointer to the focus element, if there is one in the
    // TransferSet, or zero if there is no focus element.
    // See Inserter::AddFocusCopy below for an explanation of the term
    // "focus element".
    // Note: Paste does NOT change the selection.

    auto startCopy() -> Copier;
    // Deletes the actual TransferSet content and returns an Copier.
    // The client may then insert copies of model elements into the TransferSet
    // through the Copier.
    // There may only be one active Copier per TransferSet at a time.
    // When the Copier is completed(), it will execute all necessary
    // actions which are needed to finalize the copy. For example, the
    // Pointers of the inserted model elements must be corrected to
    // point to the elements in the TransferSet. This can only be done until
    // the complete set of the to-be-copied-elements is known.

    bool isEmpty() const { return elements_.empty(); }

    auto focus() const -> IElement* { return focus_; }

    auto getGroup() -> Group&;

    void assignIDs();

private:
    void clear();
};


class TransferSet::Copier
{
    friend class TransferSet;

    TransferSet& transferSet_;
    std::unique_ptr<CopyRegistry> copyRegistry_; // ownership

public:
    Copier(const Copier&) = delete;
    Copier& operator=(const Copier&) = delete;

    auto addCopy(IElement& m) -> IElement&;
    // Adds a copy of m to the TransferSet which this inserter is bound to.
    // The client must call Complete after the last element has been added.
    // Returns ref to copy

    void addFocusCopy(IElement& focus);
    // Same as AddCopy, but this element is designated as the focus.
    // The TransferSet will be able to say which element is the focus
    // element on a later paste. The focus element is just one specially
    // marked element - nothing more. Focus elements are used by some tasks.

    void complete();
    // Must be called after the last element has been added. This
    // completes a copy operation. After Complete has been called, no more
    // elements may be added with AddCopy or AddFocusCopy.
    // The copier may safely be destructed without calling Complete, but
    // this should only happen in an error case as with an exception.
    // If the copier is destructed without a prior call to Complete,
    // the whole copy operation is aborted and the TransferSet is emptied.
    // Complete may be safely called multiple times.

private:
    Copier(TransferSet& c);
};

}
