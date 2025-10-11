/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

using namespace Exceptions;

namespace
{
using C = ObjectRegistry;
}


void C::Register(IElement& obj, ObjectID id)
{
    if (id.val() == 0)
        throw NullObjectID{};

    if (id.val() == -1)
        throw InvalidObjectID{};

    auto res = itsObjects.insert(std::make_pair(id, &obj));

    if (!res.second)
        throw DuplicateObjectID{};

    obj.SetID(id);
}


auto C::Lookup(ObjectID id) const -> IElement*
{
    if (id.val() == 0)
        return nullptr;

    if (id.val() == -1)
        throw InvalidObjectID{};

    auto iter = itsObjects.find(id);

    if (iter != end(itsObjects))
        return iter->second;

    return nullptr;
}

}
