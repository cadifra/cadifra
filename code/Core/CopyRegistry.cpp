/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;


namespace Core
{


namespace
{

class CRimp: public CopyRegistry
{
    using Map = std::map<const IElement*, IElement*>;
    Map itsMap;

public:
    //-- CopyRegistry

    void Register(const IElement* original, IElement* copy) final;
    IElement* FindCopy(const IElement* original) const  final;

    //--

    CRimp() {}
};

}


void CRimp::Register(const IElement* original, IElement* copy)
{
    D1_ASSERT(original);
    D1_ASSERT(copy);
    const bool inserted = itsMap.insert(std::make_pair(original, copy)).second;
    D1_ASSERT(inserted);
}


IElement* CRimp::FindCopy(const IElement* original) const
{
    auto i = itsMap.find(original);
    if (i == end(itsMap))
        return nullptr;

    D1_ASSERT((*i).first == original);

    auto* res = (*i).second;

    D1_ASSERT(res)

    return res;
}


auto CopyRegistry::MakeNew() -> std::unique_ptr<CopyRegistry>
{
    return std::make_unique<CRimp>();
}

}
