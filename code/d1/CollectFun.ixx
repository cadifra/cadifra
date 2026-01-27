/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CollectFun;


namespace d1
{

export template <class E>
struct CollectFun
{
    size_t count = 0;
    E first = {};
    E last = {};

    CollectFun() {}
    CollectFun(const CollectFun&) = delete;

    auto get()
    {
        return [&](E e) {
            if (not count++)
                first = e;
            last = e;
        };
    }
};

}
