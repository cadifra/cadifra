/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Canvas.Order;

import d1.types;

import std;


namespace Canvas
{

export class IOrder
{
public:
    using Ref = std::shared_ptr<IOrder>;

    virtual ~IOrder() = default;

    static Ref createMax();
    static Ref createMin();

    bool operator<(const IOrder& o) const;

private:
    virtual bool isLessThan(const IOrder&) const = 0;

    virtual bool isMax() const { return false; }
    virtual bool isMin() const { return false; }
};


export class Order // value-semantics
{
    IOrder::Ref base_;
    d1::int32 level_;

public:
    Order(
        IOrder::Ref base = IOrder::createMax(),
        d1::int32 level = 0):

        base_{ base },
        level_{ level }
    {
    }

    bool operator<(const Order& o) const;

    Order(const Order&) = default;
    Order& operator=(const Order&) = default;
};


struct MinMax: IOrder
{
    MinMax()
    {
    }

    bool isLessThan(const IOrder&) const
    {
        D1_ASSERT(false);
        return false;
    }
};


auto IOrder::createMax() -> Ref
{
    struct Max: MinMax
    {
        bool isMax() const { return true; }
    };

    static auto max = std::make_shared<Max>();
    return max;
}


auto IOrder::createMin() -> Ref
{
    struct Min: MinMax
    {
        bool isMin() const { return true; }
    };

    static auto min = std::make_shared<Min>();
    return min;
}


bool IOrder::operator<(const IOrder& o) const
{
    if (isMin())
        return not o.isMin();

    if (isMax())
        return false;

    if (o.isMin())
        return false;

    if (o.isMax())
        return true;

    return isLessThan(o);
}


bool Order::operator<(const Order& o) const
{
    if (*base_ < *o.base_)
        return true;

    if (*o.base_ < *base_)
        return false;

    return level_ < o.level_;
}

}
