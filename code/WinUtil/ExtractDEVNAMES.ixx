/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */


export module WinUtil.ExtractDEVNAMES;

import d1.wintypes;

import std;


namespace WinUtil
{

export d1::HGLOBAL createDEVNAMES(
    const std::wstring& device,
    const std::wstring& driver,
    const std::wstring& output);


export class ExtractDEVNAMES
{
public:
    std::wstring device;
    std::wstring driver;
    std::wstring output;
    ExtractDEVNAMES(d1::HGLOBAL g);
};

}
