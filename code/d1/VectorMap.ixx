/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module d1.VectorMap;

import std;


namespace d1
{

export template <class Key, class Ty, class Pr = std::less<Key>>
class VectorMap
{
    using Entry = std::pair<Key, Ty>;

    struct pred
    {
        bool operator()(const Entry& l, const Key& r) const
        {
            return Pr{}(l.first, r);
        }
    };

    std::vector<Entry> v_;

public:
    void insert(const Key& k, Ty t)
    {
        auto i = std::lower_bound(v_.cbegin(), v_.cend(), k, pred{});
        D1_ASSERT(i == v_.end() or Pr{}(k, i->first));
        v_.insert(i, Entry{ k, t });
    }

    Ty get(const Key& k)
    {
        auto i = std::lower_bound(v_.cbegin(), v_.cend(), k, pred{});
        if (i != v_.end() and not Pr{}(k, i->first))
            return i->second;
        else
            return Ty{};
    }

    void erase(const Key& k)
    {
        auto i = std::lower_bound(v_.cbegin(), v_.cend(), k, pred{});
        D1_ASSERT(i != v_.end() and not Pr{}(i->first, k));
        v_.erase(i);
    }
};


}
