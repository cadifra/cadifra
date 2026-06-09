/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Contains;

import Core.Main;

namespace Core
{

export template <class T>
class Contains
{
    const ElementSet& set_;
    bool found_ = false;

public:
    explicit Contains(const ElementSet& s):
        set_{ s }
    {
    }

    auto get()
    {
        return [&](T* t) {
            if (found_)
                return;
            found_ = set_.contains(*t);
        };
    }

    bool found() const { return found_; }
};

}
