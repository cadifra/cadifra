/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CastIteratorImp;

import d1.Iterator;

import std;


namespace d1
{

export template <class I, class Ptr, class T>
class CastIteratorImp: public d1::Iterator<T>::Imp
{
    using Base = d1::Iterator<T>::Imp;

    I iter_;

    //-- Base

    auto clone() const -> std::unique_ptr<Base> override
    {
        return std::make_unique<CastIteratorImp>(iter_);
    }

    bool isEqual(const Base& rhs) const override
    {
        return iter_ == static_cast<const CastIteratorImp&>(rhs).iter_;
    }

    void next() override { ++iter_; }

    T val() const override
    {
        return dynamic_cast<T>(static_cast<Ptr>(*iter_));
    }

    //--

public:
    CastIteratorImp(I i):
        iter_{ i }
    {
    }
};


}
