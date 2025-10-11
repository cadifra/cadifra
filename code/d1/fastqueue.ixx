/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

// a fast queue that does not move its elements on pop()

export module d1.fastqueue;

import std;


namespace d1
{

export template <std::equality_comparable T>
class fastqueue
{
    using V = std::vector<T>;

public:
    using value_type = V::value_type;
    using size_type = V::size_type;

private:
    V v_;
    size_type front_ = 0;

public:
    fastqueue() {}

    const T& front() const
    {
        return v_[front_];
    }

    T& front()
    {
        return v_[front_];
    }

    void push_back(const T& v)
    {
        v_.push_back(v);
    }

    void pop()
    {
        ++front_;
    }

    size_type size() const
    {
        return v_.size() - front_;
    }

    bool empty() const
    {
        return 0 == size();
    }

    void reserve(size_type count)
    {
        v_.reserve(count + front_);
    }

    void clear()
    {
        v_.clear();
        front_ = 0;
    }
};

}
