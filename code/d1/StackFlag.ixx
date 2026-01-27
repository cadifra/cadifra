/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

export module d1.StackFlag;


namespace d1
{

export class StackFlag
{
public:
    class Ref;

private:
    Ref* ref_;

public:
    StackFlag(Ref& r);
    ~StackFlag();

    StackFlag(const StackFlag& rhs) = delete;
    StackFlag& operator=(const StackFlag& rhs) = delete;
};


class StackFlag::Ref
{
    friend class StackFlag;
    StackFlag* stackFlag_ = nullptr;

public:
    Ref() = default;
    Ref(const Ref& rhs) {}
    ~Ref();

    operator bool() const { return stackFlag_ != nullptr; }

    Ref& operator=(const Ref& rhs) = delete;
};


inline StackFlag::StackFlag(Ref& r):
    ref_{ &r }
{
    D1_ASSERT(not ref_->stackFlag_);
    ref_->stackFlag_ = this;
}

inline StackFlag::~StackFlag()
{
    if (ref_)
        ref_->stackFlag_ = nullptr;
}

inline StackFlag::Ref::~Ref()
{
    if (stackFlag_)
        stackFlag_->ref_ = nullptr;
}

}
