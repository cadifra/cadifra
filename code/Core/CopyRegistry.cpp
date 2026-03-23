/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core:Base;


namespace Core
{


namespace
{

class CRimp: public CopyRegistry
{
    using Map = std::map<const IElement*, IElement*>;
    Map map_;

public:
    //-- CopyRegistry

    void addMapping(const IElement* original, IElement* copy) final;
    IElement* findCopy(const IElement* original) const  final;

    //--

    CRimp() {}
};

}


void CRimp::addMapping(const IElement* original, IElement* copy)
{
    D1_ASSERT(original);
    D1_ASSERT(copy);
    const bool inserted = map_.insert(std::pair{ original, copy }).second;
    D1_ASSERT(inserted);
}


IElement* CRimp::findCopy(const IElement* original) const
{
    auto i = map_.find(original);
    if (i == end(map_))
        return nullptr;

    D1_ASSERT((*i).first == original);

    auto* res = (*i).second;

    D1_ASSERT(res)

    return res;
}


auto CopyRegistry::makeNew() -> std::unique_ptr<CopyRegistry>
{
    return std::make_unique<CRimp>();
}

}
