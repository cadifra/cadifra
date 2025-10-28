/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.QueryInterface;


import d1.AutoComPtr;


export namespace WinUtil
{

template <class T1, class T2>
HRESULT QueryInterface(
    const d1::AutoComPtr<T1>& t1,
    d1::AutoComPtr<T2>& t2)
{
    return t1->QueryInterface(__uuidof(T2), &t2);
}

}
