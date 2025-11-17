/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.UTF8locale;

import d1.UTF8cvt;

import std;


namespace d1
{

std::locale UTF8locale()
{
    auto cvt = std::make_unique<d1::UTF8cvt>();
    const auto& classic = std::locale::classic();
    auto loc = std::locale{ classic, cvt.release() };
    return loc;
}

}
