/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CircularChain;

import std;


namespace d1
{

export class CircularChain
{
    CircularChain* itsNext = this;
    CircularChain* itsPrev = this;

public:
    CircularChain() = default;
    ~CircularChain() { Unlink(); }

    CircularChain(const CircularChain& r) = delete;
    CircularChain& operator=(const CircularChain&) = delete;

    bool IsUnique() const { return itsNext == this; }

    void Link(CircularChain& cc)
    {
        // symmetric: a.Link(b) and b.Link(a) do the same
        std::swap(itsNext->itsPrev, cc.itsNext->itsPrev);
        std::swap(itsNext, cc.itsNext);
    }

    void Unlink()
    {
        // doesn't change anything if it is already unique
        itsPrev->itsNext = itsNext;
        itsNext->itsPrev = itsPrev;
        itsNext = this;
        itsPrev = this;
    }
};

}
