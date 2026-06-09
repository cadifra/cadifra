/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IUndoerCollector;

namespace Core
{

export
{
class UndoerRef;
}

export class IUndoerCollector
{
public:
    virtual void add(UndoerRef) = 0;

protected:
    ~IUndoerCollector() = default;
};

}
