/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil:SEHTranslator;


namespace WinUtil::SEHTranslator
{

void ThrowCException(const EXCEPTION_RECORD& er);

}
