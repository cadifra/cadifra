/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Names;

import xml.Namespace;

import std;


namespace Core
{

using std::wstring;

namespace Elements
{

export namespace Diagram
{
const wstring& name();
const wstring& attrib_ProductVersion();
}

export namespace Object
{
const wstring& attrib_id();
}

}

export namespace Namespace
{
auto v1_0() -> const xml::Namespace&;
auto v1_3() -> const xml::Namespace&;
}


namespace Elements
{

const wstring& Diagram::name()
{
    static auto s = wstring(L"Diagram");
    return s;
}

const wstring& Diagram::attrib_ProductVersion()
{
    static auto s = wstring(L"ProductVersion");
    return s;
}

const wstring& Object::attrib_id()
{
    static auto s = wstring(L"id");
    return s;
}

}


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
