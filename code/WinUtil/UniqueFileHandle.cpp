/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil.UniqueHandle;


namespace WinUtil
{

void FileHandleWrapper::Del::operator()(FileHandleWrapper fhw) const
{
    D1_VERIFY(::CloseHandle(fhw));
}


void CloseHandleOp::operator()(d1::HANDLE h)
{
    D1_VERIFY(::CloseHandle(h));
}


void DestroyMenuOp::operator()(d1::HMENU m)
{
    D1_VERIFY(::DestroyMenu(m));
}

}
