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
    T T_;
    bool valid_ = false;

public:
    ValidValue() {}

    ValidValue(const T& t):
        T_{ t }, valid_{ true }
    {
    }

    auto& operator=(const T& t)
    {
        T_ = t, valid_ = true;
        return *this;
    }

    ValidValue(const ValidValue&) = default;

    ValidValue& operator=(const ValidValue&) = default;

    const T& val() const
    {
        D1_ASSERT(valid_);
        return T_;
    }

    bool isValid() const { return valid_; }

    void invalidate() { valid_ = false; }
};

}
