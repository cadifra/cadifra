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

    I itsIter;

    //-- Base

    auto Clone() const -> std::unique_ptr<Base> override
    {
        return std::make_unique<CastIteratorImp>(itsIter);
    }

    bool IsEqual(const Base& rhs) const override
    {
        return itsIter == static_cast<const CastIteratorImp&>(rhs).itsIter;
    }

    void Next() override { ++itsIter; }

    T Val() const override
    {
        return dynamic_cast<T>(static_cast<Ptr>(*itsIter));
    }

    //--

public:
    CastIteratorImp(I i):
        itsIter{ i }
    {
    }
};


}
