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

    // PRE: imp_ not zero
    auto& operator++(); // prefix ++

    auto operator++(int); // postfix ++

    // PRE: imp_ not zero
    T operator*() const;

private:
    std::unique_ptr<Imp> imp_;
};


template <class T>
class Iterator<T>::Imp
{
    friend class Iterator;

public:
    virtual ~Imp() = default;

protected:
    using value_type = T;

    virtual auto clone() const -> std::unique_ptr<Imp> = 0;

    virtual bool isEqual(const Imp& rhs) const = 0;
    // true -> points to same element in container

    virtual void next() = 0;
    // Advance Position by one

    virtual T val() const = 0;
    // Get Element at actual Position
};


template <class T>
Iterator<T>::Iterator(std::unique_ptr<Imp> imp):
    imp_{ std::move(imp) }
{
}


template <class T>
Iterator<T>::Iterator(const Iterator<T>& rhs)
{
    imp_ = rhs.imp_ ? rhs.imp_->clone() : nullptr;
}


template <class T>
auto& Iterator<T>::operator=(const Iterator<T>& rhs)
{
    if (this != &rhs)
    {
        imp_ = rhs.imp_ ? rhs.imp_->clone() : nullptr;
    }
    return *this;
}


template <class T>
bool Iterator<T>::operator==(const Iterator<T>& rhs) const
{
    if (not imp_ and not rhs.imp_)
        return true;
    if (not imp_ or not rhs.imp_)
        return false;
    return imp_->isEqual(*rhs.imp_);
}


template <class T>
auto& Iterator<T>::operator++() // prefix ++
{
    D1_ASSERT(imp_);
    imp_->next();
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
    D1_ASSERT(imp_);
    return imp_->val();
}

}
