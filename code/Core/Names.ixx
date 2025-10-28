/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Names;

import xml.Name;

import std;


namespace Core
{

namespace Elements
{

using std::wstring;

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

}
