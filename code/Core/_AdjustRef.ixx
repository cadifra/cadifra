module;

#include "d1/d1assert.h"

export module Core.AdjustRef;

import Core.Main;

import d1.PtrCont;


namespace Core
{

export template <class T>
void adjustRef(d1::PtrCont<T>& c, const CopyRegistry& r)
{
    auto new_set = d1::PtrCont<T>{};
    new_set.reserve(c.size());

    for (auto* ele : c)
    {
        if (auto* me = r.findCopy(ele))
        {
            auto new_ele = dynamic_cast<T*>(me);
            D1_ASSERT(new_ele);
            new_set.push_back(new_ele);
        }
    }

    c.assign_range(new_set);
}


export template <class T>
void adjustRef(T*& ptr, const CopyRegistry& r)
{
    if (auto* me = r.findCopy(ptr))
    {
        auto* new_ptr = dynamic_cast<T*>(me);
        D1_ASSERT(new_ptr);
        ptr = new_ptr;
    }
    else
    {
        ptr = 0;
    }
}

}
