/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Attachment;

import Core.ITerminal;
import Core.Interfaces;


namespace Core
{

export class IAttachment: public virtual ITerminal
{
public:
    bool isBound() const { return getTerminal() != 0; }

    virtual auto getTerminal() const -> ITerminal* = 0; // may return 0

    virtual void forget(Env&, ITerminal&, bool isDeleteRequest) = 0;

    virtual void terminalReshaped(Env&,
        const ReshapeInfo&, const ShiftSet&) = 0;
};


// A IPointAttachment is an Attachment which has the form of a point.
// Examples for IPointAttachment are all kinds of LinkEnds, TextAnchors, ...
//
export class IPointAttachment:
    public virtual IAttachment,
    public virtual IPosition
{
public:
    //-- ITerminal

    auto findNearestPointImpl(const d1::fPoint& p, bool horizontal) const
        -> NearestRes override
    {
        return { pos(), {} };
    }

protected:
    IPointAttachment() {}
};

}
