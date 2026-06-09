/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Env;

import :Transaction;
import :Undoer;

import Core.Grid;


namespace Core
{

export {
class Transaction;
class IDiagram;
class IView;
class UndoerRef;
namespace Selection
{
class Tracker;
}
}


export struct Env
{
    Transaction& transaction;
    Selection::Tracker& sel_tracker;
    const IGrid& grid;

    auto diagram() const -> IDiagram&
    {
        return transaction.diagram();
    }

    auto workingView() const -> IView*
    {
        return transaction.workingView();
    }

    void addNewlyCreated(IElementRef me)
    {
        transaction.addNewlyCreated(me);
    }

    void addTouched(IElement& me, bool update_view)
    {
        transaction.addTouched(me, update_view);
    }

    auto close() -> UndoerRef
    {
        return transaction.close(sel_tracker, grid);
    }
};

}
