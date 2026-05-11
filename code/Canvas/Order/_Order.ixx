/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

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

}
