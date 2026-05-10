/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Diagram;

import :Element;
import :Undoer;

import Core.IDirtyMarker;
import Core.IDocumentChange;
import Core.IUndoRedoCountChange;
import Core.ObjectID;

import d1.Iterator;
import d1.types;
import d1.Observer;
import d1.Point;

import std;


namespace Core
{

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

}
