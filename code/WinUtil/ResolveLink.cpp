/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <shlobj.h>

module WinUtil.ResolveLink;

import WinUtil.QueryInterface;

import d1.AutoComPtr;
import d1.buffer;

import std;


namespace WinUtil
{


std::wstring resolveLink(const std::wstring& path, HWND hwnd, bool fast)
{
    if (path.empty())
        return path;

    CLSID clsid;
    if (S_OK != ::GetClassFile(path.c_str(), &clsid))
        return path;

    if (clsid != CLSID_ShellLink)
        return path;

    auto sl = d1::AutoComPtr<IShellLink>{};

    HRESULT res = ::CoCreateInstance(
        CLSID_ShellLink,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        &sl);

    if (FAILED(res))
        return path;

    auto pf = d1::AutoComPtr<IPersistFile>{};

    QueryInterface(sl, pf);

    if (not pf)
        return path;

    res = pf->Load(path.c_str(), STGM_READWRITE);

    if (FAILED(res))
        return path;

    if (not fast)
    {
        res = sl->Resolve(hwnd, SLR_ANY_MATCH | SLR_UPDATE);

        if (res == S_FALSE)
            return {};

        if (FAILED(res))
            return path;
    }

    auto buf = d1::wbuffer(MAX_PATH + 1);

    res = sl->GetPath(
        buf.data(),
        static_cast<int>(buf.size()),
        0,
        0
    );

    if (res != S_OK)
        return path;

    return buf.data();
}


}
