/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Attach;

import Core.Interfaces;
import Core.Shift;

import Core.Main;

import d1.Point;


namespace Core
{

export class ITerminal: public virtual IShiftable
//
// IAttachment's can be attached to ITerminal's through template class IPartner<T>.
// ITerminal is also used as base for dynamic_casts to concrete IPartner<T> classes.
//
{
public:
    using NearestRes = std::pair<
        d1::fPoint,   // nearest point on envelope
        d1::fVector>; // perpendicular to envelope, in outside direction

    auto findNearestPoint(const ShiftSet&, const d1::fPoint& p,
        bool horizontal) const -> NearestRes;
    // Finds the nearest intersection of the terminal's boundary with a
    // horizontal (or vertical if "horizontal == false) line through p.
    // If no such intersection exists, FindNearestPoint returns the nearest
    // point on the boundary.

    virtual auto findNearestPointImpl(const d1::fPoint& p,
        bool horizontal) const -> NearestRes = 0;

    d1::fPoint nearest(const d1::fPoint& p) const;

    class ReshapeInfo;
};


class ITerminal::ReshapeInfo
{
public:
    auto getNewAttachmentPos(const d1::fPoint& oldPos,
        const d1::fPoint& farPos, const ShiftSet&) const -> d1::fPoint;
    // calls GetNewAttachmentPosImpl

private:
    virtual auto getNewAttachmentPosImpl(const d1::fPoint& oldPos,
        const d1::fPoint& farPos) const -> d1::fPoint = 0;

public:
    virtual auto getReshapedTerminal() const -> const ITerminal& = 0;

protected:
    ~ReshapeInfo() = default;
};


export class IAttachment: public virtual ITerminal
{
public:
    bool isBound() const { return getTerminal() != 0; }

    virtual auto getTerminal() const -> ITerminal* = 0; // may return 0

    virtual void forget(Env&, ITerminal&, bool isDeleteRequest) = 0;

    virtual void terminalReshaped(Env&,
        const ReshapeInfo&, const ShiftSet&) = 0;
};


// A IPointAttachment is an Attachment which has the form of point.
// Examples for IPointAttachment are all kinds of LinkEnds, TextAnchors, ...
//
export class IPointAttachment:
    public virtual IAttachment,
    public virtual IPosition
{
public:
    //-- ITerminal

    auto findNearestPointImpl(const d1::fPoint& p, bool horizontal) const
        -> NearestRes override;

    //--

protected:
    IPointAttachment() {}
};


export template <class E>
class IAttach: public virtual IElement
//
// Model element that supports the attaching of objects of type E to it
// (typically a specific end type).
// Examples for type E: AssocEnd, GeneralizationEnd
//
{
public:
    virtual bool testAttach(const E* end) const = 0;
    // returns true, if this objects currently accepts attaching the given
    // end. end may be zero. If end is zero, the called object is asked,
    // whether it currently accepts attaching elements of types E.

    bool attach(Core::Env& e, E& end)
    // Attach end to this object if TestAttach returns true and return
    // result of TestAttach.
    // PRE: the Position of end must already be on the boundary of this
    //      element (graphically).
    {
        if (not testAttach(&end))
            return false;
        attachImp(e, end);
        return true;
    }

private:
    virtual void attachImp(Core::Env&, E& end) = 0;
};


export template <class END>
class AttachFilter: public IFilter
{
    const END* end_;

public:
    AttachFilter(const END* end = 0):
        end_{ end }
    {
    }

    bool pass(const VIPointable& p) const override
    {
        const auto ve = dynamic_cast<const IViewElement*>(&p);
        if (ve)
        {
            const auto& me = ve->element();
            const auto att = dynamic_cast<const IAttach<END>*>(&me);
            if (att and att->testAttach(end_))
                return true;
        }
        return false;
    }
};

}
