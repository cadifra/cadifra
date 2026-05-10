/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.IUndoRedoCountChange;

namespace Core
{

export class IUndoRedoCountChange
{
public:
    virtual void undoRedoCountChanged(int UndoCount, int RedoCount) = 0;

protected:
    ~IUndoRedoCountChange() = default;
};

}
