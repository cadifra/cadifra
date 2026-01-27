/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CircularChain;

import std;


namespace d1
{

export class CircularChain
{
    CircularChain* next_ = this;
    CircularChain* prev_ = this;

public:
    CircularChain() = default;
    ~CircularChain() { unlink(); }

    CircularChain(const CircularChain& r) = delete;
    CircularChain& operator=(const CircularChain&) = delete;

    bool isUnique() const { return next_ == this; }

    void link(CircularChain& cc)
    {
        // symmetric: a.link(b) and b.link(a) do the same
        std::swap(next_->prev_, cc.next_->prev_);
        std::swap(next_, cc.next_);
    }

    void unlink()
    {
        // doesn't change anything if it is already unique
        prev_->next_ = next_;
        next_->prev_ = prev_;
        next_ = this;
        prev_ = this;
    }
};

}
