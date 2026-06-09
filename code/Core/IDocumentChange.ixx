/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.IDocumentChange;

namespace Core
{

export class IDocumentChange
{
public:
    virtual void documentChanged() = 0;

protected:
    ~IDocumentChange() = default;
};

}
