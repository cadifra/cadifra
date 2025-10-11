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
class VectorMap: private std::vector<std::pair<Key, Ty>>
{
    using Entry = std::pair<Key, Ty>;

    struct pred
    {
        bool operator()(const Entry& l, const Key& r) const
        {
            return Pr{}(l.first, r);
        }
    };

public:
    void Insert(const Key& k, Ty t)
    {
        auto i = std::lower_bound(this->begin(), this->end(), k, pred{});
        D1_ASSERT(i == this->end() || Pr{}(k, i->first));
        this->insert(i, Entry{ k, t });
    }

    Ty Get(const Key& k)
    {
        auto i = std::lower_bound(this->begin(), this->end(), k, pred{});
        if (i != this->end() && !Pr{}(k, i->first))
            return i->second;
        else
            return Ty{};
    }

    void Erase(const Key& k)
    {
        auto i = std::lower_bound(this->begin(), this->end(), k, pred{});
        D1_ASSERT(i != this->end() && !Pr{}(i->first, k));
        this->erase(i);
    }
};


}
