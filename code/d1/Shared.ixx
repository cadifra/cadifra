/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Shared;

import std;

namespace d1
{

export class Shared: public std::enable_shared_from_this<Shared>
{
public:
    Shared() = default;

    Shared operator=(const Shared&) = delete;

    virtual ~Shared() = 0;
};


inline Shared::~Shared() = default;

}
