/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Util;

import std;

namespace Core
{

export template <typename T, typename C = std::vector<T*>>
class PtrCont: public C
{
public:
    using Eletype = T;

    PtrCont() {}
};

}
