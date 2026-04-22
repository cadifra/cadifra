/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.PtrCont;

import std;

namespace d1
{

export template <typename T, typename C = std::vector<T*>>
class PtrCont: public C
{
public:
    using Eletype = T;

    PtrCont() {}
};

}
