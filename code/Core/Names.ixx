/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Names;

import xml.Name;

import std;


namespace Core
{

namespace Elements
{

using std::wstring;

export class Diagram
{
public:
    static const wstring& name();
    static const wstring& attrib_ProductVersion();
};

export class Object
{
public:
    static const wstring& attrib_id();
};

}


export class Namespace
{
public:
    static auto v1_0() -> const xml::Namespace&;
    static auto v1_3() -> const xml::Namespace&;
};

}
