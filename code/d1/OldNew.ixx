/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.OldNew;

namespace d1
{

export struct OldNew
{
    enum T
    {
        new_val = 0,
        old_val
    };

    T v_;

    constexpr OldNew():
        v_(new_val) {}
    constexpr explicit OldNew(T v):
        v_(v) {}
    bool old() const { return v_ == old_val; }
};

export constexpr OldNew New;
export constexpr OldNew Old{ OldNew::old_val };

}
