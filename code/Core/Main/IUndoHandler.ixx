/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IUndoHandler;

import :SelectionRestorer;

import Core.IDocumentChange;
import Core.IUndoRedoCountChange;

import d1.Observer;


namespace Core
{

export
{
class IDiagram;
class UndoerRef;
class IDirtyMarker;
}

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

}
