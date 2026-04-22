/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core:CopyRegistry;

import d1.PtrCont;

import std;


namespace Core
{

export class IElement;


export class CopyRegistry
//
// Maintains a mapping from original objects to their copies.
//
{
public:
    static auto makeNew() -> std::unique_ptr<CopyRegistry>;

    virtual ~CopyRegistry() = default;

    virtual void addMapping(const IElement* original, IElement* copy) = 0;
    // Stores the mapping from original to copy (ref only pointers).
    // PRE: (1) original != 0 and copy != 0
    //      (2) original and copy have not been registered yet (neither as a copy
    //          nor as an original object)

    virtual IElement* findCopy(const IElement* original) const = 0;
    // Searches original in the Registry and returns the pointer to its copy if
    // found, zero if not found.
};


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
