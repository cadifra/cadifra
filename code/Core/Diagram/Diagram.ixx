/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Diagram;

import :Base;
import :Undoer;
import :Selection;

import d1.Iterator;
import d1.types;
import d1.Observer;
import d1.Point;

import std;


namespace Core
{

export class IDirtyStateChange
{
public:
    virtual void dirtyStateChanged(bool IsDirty) = 0;

protected:
    ~IDirtyStateChange() = default;
};


export class IDirtyMarker
{
public:
    IDirtyMarker() = default;
    IDirtyMarker(const IDirtyMarker&) = delete;
    IDirtyMarker& operator=(const IDirtyMarker&) = delete;

    virtual ~IDirtyMarker() = default;

    virtual bool isDirty() const = 0;

    virtual void clearDirtyFlag(bool notifyObservers = true) = 0;

    virtual auto getDirtyStateConnector()
        -> d1::Observer::C<IDirtyStateChange> = 0;
};


export class IDocumentChange
{
public:
    virtual void documentChanged() = 0;

protected:
    ~IDocumentChange() = default;
};


export class IUndoRedoCountChange
{
public:
    virtual void undoRedoCountChanged(int UndoCount, int RedoCount) = 0;

protected:
    ~IUndoRedoCountChange() = default;
};


export class IUndoHandler
{
    IDiagram& diagram_;

protected:
    IUndoHandler(IDiagram& d):
        diagram_{ d }
    {
    }

public:
    virtual ~IUndoHandler() = default;

    IDiagram& getDiagram() const { return diagram_; }

    virtual void add(UndoerRef u, Selection::IRestorerRef pre,
        Selection::IRestorerRef post, bool merge = false) = 0;
    // Add an Undoer to this UndoHandler. This UndoHandler becomes the owner of
    // the added undoer. The added Undoer is deleted, when this UndoHandler is
    // deleted.
    // 'pre' is responsible to restore the selection state as it was,
    // BEFORE the execution of the user's command that lead to the
    // production of the Undoer u. When this command is undone, the UndoHandler
    // must restore this selection in the active diagram view.
    // 'post' is responsible to restore the selection state as it was,
    // AFTER the execution of the user's command that lead to the
    // production of the Undoer u. When this command is redone, the UndoHandler
    // must restore this selection in the active diagram view.
    // If merge is true, the UndoHandler tries to merge the Undoer u with
    // the Undoer on the top of the undo stack.

    virtual void undo(IView& activeView, Selection::Tracker&, int numOfUndos) = 0;
    virtual void redo(IView& activeView, Selection::Tracker&, int numOfRedos) = 0;

    virtual int undoCount() const = 0;
    virtual int redoCount() const = 0;


    virtual auto createDirtyMarker() -> std::unique_ptr<IDirtyMarker> = 0;

    virtual auto getDocumentChangeConnector()
        -> d1::Observer::C<IDocumentChange> = 0;

    virtual auto getUndoRedoCountChangeConnector()
        -> d1::Observer::C<IUndoRedoCountChange> = 0;
};


export class IPastePostProcessor
{
public:
    virtual void pastePostProcessing(
        Env&, const ElementSet& pasted_elements) const = 0;

protected:
    ~IPastePostProcessor() = default;
};


export class IDiagram: public IClub
{
    const IPastePostProcessor* const pastePostProcessor_; // may be zero

public:
    IDiagram(const IPastePostProcessor* p):
        pastePostProcessor_(p) {}
    IDiagram(const IDiagram&) = delete;
    IDiagram& operator=(const IDiagram&) = delete;

    virtual ~IDiagram() = default;

    auto* getPastePostProcessor() const { return pastePostProcessor_; }

    virtual void add(IView&) = 0;
    virtual void forget(IView&) = 0;
    virtual auto getActiveView() const -> IView* = 0;

    virtual auto getUndoHandler() const -> IUndoHandler& = 0;

    virtual d1::uint32 getNumOfElements() const = 0;
    // returns the number of elements, which are in this diagram. returns
    // 0, if no diagram is opened.

    virtual auto getNamedElement(const std::wstring& name) const -> IElement* = 0;

    virtual void insert(IElementRef) = 0;
    virtual void remove(IElement&) = 0;

    virtual void assignIDs() = 0;
    // assigns ObjectIDs to all elements in the diagram that have an ID of zero

    virtual void createViewElements(IElement&, bool update_views = true) = 0;

    virtual void destroyViewElements(IElement&, Selection::Tracker&, IView* active_v = 0) = 0;

    virtual void updateViews() = 0;

    virtual bool hasRunningTransaction() const = 0;
    virtual void add(Transaction&) = 0;
    virtual void forget(Transaction&) = 0;

    virtual bool hasDoingUndoer() const = 0;
    virtual void beginUndoRedo(Undoer&) = 0;
    virtual void endUndoRedo(Undoer&) = 0;

    virtual void check() const = 0;
};


export class Group
{
public:
    virtual void assignIDs() = 0;

    virtual auto begin() const -> d1::Iterator<IElement*> = 0;
    virtual auto end() const -> d1::Iterator<IElement*> = 0;

    Group() = default;
    virtual ~Group() = default;

    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    friend auto begin(const Group& g)
    {
        return g.begin();
    }

    friend auto end(const Group& g)
    {
        return g.end();
    }
};


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


namespace CopyExtendSelection
{

export auto build(const ElementSet& selection, IDiagram&) -> ElementSet;

}

}
