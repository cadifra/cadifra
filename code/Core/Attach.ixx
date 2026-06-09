/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Attach;

import Core.Main;


namespace Core
{

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
