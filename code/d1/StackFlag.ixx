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
    friend class Ref;
    Ref* itsRef;

public:
    StackFlag(Ref& r);
    ~StackFlag();

    StackFlag(const StackFlag& rhs) = delete;
    StackFlag& operator=(const StackFlag& rhs) = delete;
};


class StackFlag::Ref
{
    friend class StackFlag;
    StackFlag* itsStackFlag = nullptr;

public:
    Ref() = default;
    Ref(const Ref& rhs) {}
    ~Ref();

    operator bool() const { return itsStackFlag != nullptr; }

    Ref& operator=(const Ref& rhs) = delete;
};


inline StackFlag::StackFlag(Ref& r):
    itsRef{ &r }
{
    D1_ASSERT(!itsRef->itsStackFlag);
    itsRef->itsStackFlag = this;
}

inline StackFlag::~StackFlag()
{
    if (itsRef)
        itsRef->itsStackFlag = nullptr;
}

inline StackFlag::Ref::~Ref()
{
    if (itsStackFlag)
        itsStackFlag->itsRef = nullptr;
}

}
