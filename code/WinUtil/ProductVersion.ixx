/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.ProductVersion;

import d1.types;


namespace WinUtil
{

export class ProductVersion
{
public:
    d1::uint16 first = 0, second = 0, third = 0;

    static const wchar_t separator = L'.';

    ProductVersion(d1::uint16 first, d1::uint16 second, d1::uint16 third):
        first{ first }, second{ second }, third{ third }
    {
    }

    ProductVersion() {}

    bool operator>(const ProductVersion& rhs) const;
};

}
