/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:ObjectRegistry;

import :Base;


namespace Core
{

export class IObjectRegistry
{
public:
    virtual auto getElement(ObjectID) const -> IElement* = 0;
    // Search for the Object with the given ID.
    // returns 0 if not found.

protected:
    ~IObjectRegistry() = default;
};


export class ObjectRegistry: public IObjectRegistry
{
    using Map = std::map<ObjectID, IElement*>;

    Map map_; // no ownership

public:
    //-- IObjectRegistry

    auto getElement(ObjectID) const -> IElement* override;

    //--

    void insert(IElement& obj, ObjectID id);

    ObjectRegistry() {}

    ObjectRegistry(const ObjectRegistry&) = delete;
    ObjectRegistry& operator=(const ObjectRegistry&) = delete;
};

}
