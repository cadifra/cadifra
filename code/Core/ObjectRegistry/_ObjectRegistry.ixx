/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.ObjectRegistry;

import Core.ObjectID;

import Core.Main;

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

}
