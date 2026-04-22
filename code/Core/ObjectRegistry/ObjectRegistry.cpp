/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;

import Core.Exceptions;


namespace Core
{

using namespace Exceptions;

namespace
{
using C = ObjectRegistry;
}


void C::insert(IElement& obj, ObjectID id)
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


auto C::getElement(ObjectID id) const -> IElement*
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
