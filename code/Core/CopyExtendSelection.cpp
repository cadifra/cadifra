/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;


namespace Core
{

auto CopyExtendSelection::Build(const ElementSet& selection, IDiagram& d)
    -> ElementSet
{
    struct Caller: public ExtendSelectionParam::ICaller
    {
        ExtendSelectionResult MakeCall(
            const IElement* target,
            ExtendSelectionParam& p) const final
        {
            return target->IsCopySelected(p);
        }
    } caller;

    auto p = ExtendSelectionParam{ caller, selection };
    ElementSet res;

    for (auto me : d)
    {
        D1_ASSERT(me);

        if (p.Call(me) == ExtendSelectionResult::yes)
            me->DeepInsert(res);
    }

    return res;
}

}
