/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.Color;

import std;


namespace d1
{

std::wstring Color::str() const
{
    auto ss = std::wostringstream{};
    ss << L"0x";
    ss << std::hex << std::uppercase;
    ss.width(2); ss.fill(L'0'); ss << red;
    ss.width(2); ss.fill(L'0'); ss << green;
    ss.width(2); ss.fill(L'0'); ss << blue;
    return ss.str();
}

}
