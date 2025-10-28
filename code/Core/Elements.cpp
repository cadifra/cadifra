/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core.Names;

import std;


namespace Core::Elements
{

const std::wstring& Diagram::name()
{
    static auto s = std::wstring(L"Diagram");
    return s;
}

const std::wstring& Diagram::attrib_ProductVersion()
{
    static auto s = std::wstring(L"ProductVersion");
    return s;
}

const std::wstring& Object::attrib_id()
{
    static auto s = std::wstring(L"id");
    return s;
}

}
