/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Canvas.Order;


namespace Canvas
{

namespace
{

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

}

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
