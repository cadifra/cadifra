/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;
import :Diagram;


namespace Core::CopyExtendSelection
{

auto build(const ElementSet& selection, IDiagram& d)
    -> ElementSet
{
    namespace ES = ExtendSelection;

    struct Caller: public ES::Param::ICaller
    {
        ES::Result makeCall(const IElement* target, ES::Param& p) const final
        {
            return target->isCopySelected(p);
        }
    } caller;

    auto p = ES::Param{ caller, selection };
    ElementSet res;

    for (auto me : d)
    {
        D1_ASSERT(me);

        if (p.call(me) == ES::Result::yes)
            me->deepInsert(res);
    }

    return res;
}

}
