/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:ConsumeDropHandle;

import d1.Point;
import d1.wintypes;

import std;


namespace WinUtil
{

export class ConsumeDropHandle
{
public:
    using FileList = std::vector<std::wstring>;

    d1::Point point;
    FileList fileList;

    ConsumeDropHandle(d1::HDROP);
};

}
