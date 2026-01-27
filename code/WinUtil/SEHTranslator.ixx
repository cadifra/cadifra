/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.SEHTranslator;


export namespace WinUtil::SEHTranslator
{

void throwCException(const EXCEPTION_RECORD& er);

}
