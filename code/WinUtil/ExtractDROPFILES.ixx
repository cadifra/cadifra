/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <shlobj_core.h>

export module WinUtil.ExtractDROPFILES;

import std;


namespace WinUtil
{

export struct ExtractDROPFILES
{
    ExtractDROPFILES(const ::_DROPFILES&);

    using Cont = std::vector<std::wstring>;
    Cont files;
};

}
