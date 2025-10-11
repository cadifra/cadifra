/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto Env::Diagram() const -> IDiagram&
{
    return transaction.Diagram();
}


auto Env::WorkingView() const -> IView*
{
    return transaction.WorkingView();
}


void Env::AddNewlyCreated(IElementPtr me)
{
    transaction.AddNewlyCreated(me);
}


void Env::AddTouched(IElement& me, bool update_view)
{
    transaction.AddTouched(me, update_view);
}


auto Env::Close() -> UndoerRef
{
    return transaction.Close(sel_tracker, grid);
}

}

