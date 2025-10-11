/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <shlobj.h>

module WinUtil;

import d1.stringConvert;


namespace WinUtil
{

ExtractDROPFILES::ExtractDROPFILES(const ::_DROPFILES& df)
{
    files.clear();

    if (!df.pFiles)
        return;

    const auto* p = reinterpret_cast<const std::string::value_type*>(&df);

    p += df.pFiles;

    if (df.fWide)
    {
        const auto* f = reinterpret_cast<const std::wstring::value_type*>(p);

        while (f)
        {
            auto s = std::wstring(f);

            if (!s.size())
                return;

            files.push_back(s);

            f += s.size() + 1;
        }
    }
    else
    {
        const auto* f = p;

        while (f)
        {
            auto s = std::string(f);

            if (!s.size())
                return;

            files.push_back(d1::string2wstring(s));

            f += s.size() + 1;
        }
    }
}

}
