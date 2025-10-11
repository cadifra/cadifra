/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

export module d1.ValidValue;


namespace d1
{

export template <class T>
class ValidValue
{
    T itsT;
    bool itIsValid = false;

public:
    ValidValue() {}

    ValidValue(const T& t):
        itsT{ t }, itIsValid{ true }
    {
    }

    auto& operator=(const T& t)
    {
        itsT = t, itIsValid = true;
        return *this;
    }

    ValidValue(const ValidValue&) = default;

    ValidValue& operator=(const ValidValue&) = default;

    const T& Val() const
    {
        D1_ASSERT(itIsValid);
        return itsT;
    }

    bool IsValid() const { return itIsValid; }

    void Invalidate() { itIsValid = false; }
};

}
