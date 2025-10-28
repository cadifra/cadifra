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
    namespace ES = ExtendSelection;

    struct Caller: public ES::Param::ICaller
    {
        ES::Result MakeCall(const IElement* target, ES::Param& p) const final
        {
            return target->IsCopySelected(p);
        }
    } caller;

    auto p = ES::Param{ caller, selection };
    ElementSet res;

    for (auto me : d)
    {
        D1_ASSERT(me);

        if (p.Call(me) == ES::Result::yes)
            me->DeepInsert(res);
    }

    return res;
}

}
