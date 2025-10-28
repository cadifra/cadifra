/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.CLSID;

import WinUtil.ComException;


namespace WinUtil
{

CLSID ConvertToCLSID(const wchar_t* s)
{
    auto out = CLSID{};

    const HRESULT res = ::CLSIDFromString(
        /* LPOLESTR lpsz  */ const_cast<LPOLESTR>(s)
        /*   [in]  Pointer to the string representation of the CLSID */,
        /* LPCLSID pclsid */ &out
        /*   [out] Pointer to the CLSID on return */
    );

    ComException::Check(res);

    return out;
}

}

