/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

// Abstract Iterators

module;

#include "d1assert.h"

export module d1.Iterator;

import std;


namespace d1
{

template <class>
class Iterator;


export template <class T>
class IIteratorImp
{
    friend class Iterator<T>;

public:
    virtual ~IIteratorImp() = default;

protected:
    using value_type = T;

    virtual auto Clone() const -> std::unique_ptr<IIteratorImp<T>> = 0;

    virtual bool IsEqual(const IIteratorImp& rhs) const = 0;
    // true -> points to same element in container

    virtual void Next() = 0;
    // Advance Position by one

    virtual T Val() const = 0;
    // Get Element at actual Position
};


export template <class T>
class Iterator // has value semantics
{
    std::unique_ptr<IIteratorImp<T>> itsImp;

public:
    using value_type = T;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator() = default;

    Iterator(std::unique_ptr<IIteratorImp<T>> imp):
        itsImp{ std::move(imp) }
    {
    }

    Iterator(const Iterator<T>& rhs)
    {
        itsImp = rhs.itsImp ? rhs.itsImp->Clone() : nullptr;
    }

    auto& operator=(const Iterator<T>& rhs)
    {
        if (this != &rhs)
        {
            itsImp = rhs.itsImp ? rhs.itsImp->Clone() : nullptr;
        }
        return *this;
    }

    bool operator==(const Iterator<T>& rhs) const
    {
        if (!itsImp && !rhs.itsImp)
            return true;
        if (!itsImp || !rhs.itsImp)
            return false;
        return itsImp->IsEqual(*rhs.itsImp);
    }

    // PRE: itsImp not zero
    auto& operator++() // prefix ++
    {
        D1_ASSERT(itsImp);
        itsImp->Next();
        return *this;
    }

    auto operator++(int) // postfix ++
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    // PRE: itsImp not zero
    T operator*() const
    {
        D1_ASSERT(itsImp);
        return itsImp->Val();
    }
};


}
