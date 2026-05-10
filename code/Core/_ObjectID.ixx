/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.ObjectID;

import d1.types;


namespace Core
{

export class ObjectID
{
public:
    using T = d1::uint32;

private:
    T v_ = 0;

public:
    ObjectID() {}

    explicit ObjectID(T t):
        v_{ t }
    {
    }

    T val() const
    {
        return v_;
    }

    auto& operator++()
    {
        ++v_;
        return *this;
    }

    auto operator<=>(const ObjectID& rhs) const = default;
};

}
