/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.ObjectRegistry;

import Core.ObjectID;
import Core.Main;
import Core.Exceptions;

import std;


namespace Core
{

export class ObjectRegistry
{
    using Map = std::map<ObjectID, IElement*>;

    Map map_; // no ownership

public:
    auto getElement(ObjectID) const -> IElement*;

    void insert(IElement& obj, ObjectID id);

    ObjectRegistry() {}

    ObjectRegistry(const ObjectRegistry&) = delete;
    ObjectRegistry& operator=(const ObjectRegistry&) = delete;
};


using namespace Exceptions;


void ObjectRegistry::insert(IElement& obj, ObjectID id)
{
    if (id.val() == 0)
        throw NullObjectID{};

    if (id.val() == -1)
        throw InvalidObjectID{};

    auto res = map_.insert(std::pair{ id, &obj });

    if (not res.second)
        throw DuplicateObjectID{};

    obj.setID(id);
}


auto ObjectRegistry::getElement(ObjectID id) const -> IElement*
{
    if (id.val() == 0)
        return nullptr;

    if (id.val() == -1)
        throw InvalidObjectID{};

    auto iter = map_.find(id);

    if (iter != end(map_))
        return iter->second;

    return nullptr;
}

}
