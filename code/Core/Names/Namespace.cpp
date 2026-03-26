/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core.Names;

import xml.Name;


namespace Core
{

auto Namespace::v1_0() -> const xml::Namespace&
{
    static const auto n = xml::Namespace{ L"http://www.cadifra.com/schema/1.0", L"c" };
    return n;
}

auto Namespace::v1_3() -> const xml::Namespace&
{
    static const auto n = xml::Namespace{ L"http://www.cadifra.com/schema/1.3", L"c1v3" };
    return n;
}

}
