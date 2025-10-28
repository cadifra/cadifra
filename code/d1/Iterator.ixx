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

export template <class T>
class Iterator // has value semantics
{
public:
    class Imp;

    using value_type = T;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator() = default;

    Iterator(std::unique_ptr<Imp> imp);
    Iterator(const Iterator<T>& rhs);

    auto& operator=(const Iterator<T>& rhs);

    bool operator==(const Iterator<T>& rhs) const;

    // PRE: itsImp not zero
    auto& operator++(); // prefix ++

    auto operator++(int); // postfix ++

    // PRE: itsImp not zero
    T operator*() const;

private:
    std::unique_ptr<Imp> itsImp;
};


template <class T>
class Iterator<T>::Imp
{
    friend class Iterator;

public:
    virtual ~Imp() = default;

protected:
    using value_type = T;

    virtual auto Clone() const -> std::unique_ptr<Imp> = 0;

    virtual bool IsEqual(const Imp& rhs) const = 0;
    // true -> points to same element in container

    virtual void Next() = 0;
    // Advance Position by one

    virtual T Val() const = 0;
    // Get Element at actual Position
};


template <class T>
Iterator<T>::Iterator(std::unique_ptr<Imp> imp):
    itsImp{ std::move(imp) }
{
}


template <class T>
Iterator<T>::Iterator(const Iterator<T>& rhs)
{
    itsImp = rhs.itsImp ? rhs.itsImp->Clone() : nullptr;
}


template <class T>
auto& Iterator<T>::operator=(const Iterator<T>& rhs)
{
    if (this != &rhs)
    {
        itsImp = rhs.itsImp ? rhs.itsImp->Clone() : nullptr;
    }
    return *this;
}


template <class T>
bool Iterator<T>::operator==(const Iterator<T>& rhs) const
{
    if (!itsImp && !rhs.itsImp)
        return true;
    if (!itsImp || !rhs.itsImp)
        return false;
    return itsImp->IsEqual(*rhs.itsImp);
}


template <class T>
auto& Iterator<T>::operator++() // prefix ++
{
    D1_ASSERT(itsImp);
    itsImp->Next();
    return *this;
}


template <class T>
auto Iterator<T>::operator++(int) // postfix ++
{
    auto tmp = *this;
    ++*this;
    return tmp;
}


template <class T>
T Iterator<T>::operator*() const
{
    D1_ASSERT(itsImp);
    return itsImp->Val();
}

}
