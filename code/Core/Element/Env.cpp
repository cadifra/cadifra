/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;
import :Element;

import :Undoer;
import :Transaction;


namespace Core
{

auto Env::diagram() const -> IDiagram&
{
    return transaction.diagram();
}


auto Env::workingView() const -> IView*
{
    return transaction.workingView();
}


void Env::addNewlyCreated(IElementRef me)
{
    transaction.addNewlyCreated(me);
}


void Env::addTouched(IElement& me, bool update_view)
{
    transaction.addTouched(me, update_view);
}


auto Env::close() -> UndoerRef
{
    return transaction.close(sel_tracker, grid);
}

}

