/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Cache;

namespace d1
{

export template <typename T>
class Cache
{
    Cache* invalid_; // "0" or "this"
    T value_;

public:
    Cache() { invalidate(); }

    Cache(const Cache&) { invalidate(); }

    auto& operator=(const Cache&)
    {
        invalidate();
        return *this;
    }

    T* validate() const
    {
        if (not invalid_)
            return 0;
        T* res = &invalid_->value_;
        invalid_->invalid_ = 0;
        return res;
    }

    T* invalidate()
    {
        invalid_ = this;
        return &value_;
    }

    const T& get() const { return value_; }

    bool isValid() const { return not invalid_; }
};

}
