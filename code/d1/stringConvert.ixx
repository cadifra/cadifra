/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.stringConvert;

import std;

export namespace d1
{

auto string2wstring(const std::string s) -> std::wstring;
auto wstring2string(const std::wstring ws) -> std::string;

}
