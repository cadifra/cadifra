/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil;


namespace WinUtil
{

void FileHandleWrapper::Del::operator()(FileHandleWrapper fhw) const
{
    D1_VERIFY(::CloseHandle(fhw));
}

}
