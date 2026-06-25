/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Main:CopyRegistry;

import std;


namespace Core
{

export
{
class IElement;
}

export class CopyRegistry
//
// Maintains a mapping from original objects to their copies.
//
{
    using Map = std::map<const IElement*, IElement*>;
    Map map_;

public:
    void addMapping(const IElement* original, IElement* copy);
    // Stores the mapping from original to copy (ref only pointers).
    // PRE: (1) original != 0 and copy != 0
    //      (2) original and copy have not been registered yet (neither as a copy
    //          nor as an original object)

    IElement* findCopy(const IElement* original) const;
    // Searches original in the Registry and returns the pointer to its copy if
    // found, zero if not found.
};


void CopyRegistry::addMapping(const IElement* original, IElement* copy)
{
    D1_ASSERT(original);
    D1_ASSERT(copy);
    const bool inserted = map_.insert(std::pair{ original, copy }).second;
    D1_ASSERT(inserted);
}


IElement* CopyRegistry::findCopy(const IElement* original) const
{
    auto i = map_.find(original);
    if (i == end(map_))
        return nullptr;

    D1_ASSERT((*i).first == original);

    auto* res = (*i).second;

    D1_ASSERT(res)

    return res;
}

}
