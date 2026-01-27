/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <malloc.h>

module WinUtil.GuardedFunctionCall;

import WinUtil.CException;
import WinUtil.ExceptionBox;
import WinUtil.SEHTranslator;

import d1.Exception;

import std;


namespace WinUtil
{

namespace
{
using C = GuardedFunctionCall;
}


void C::showErrorBox(const std::exception& e) const
{
    const auto name = std::string(typeid(e).name());
    auto os = std::ostream{ &WinUtil::ExceptionBox::clear() };
    os
        << name << " catched in "
        << (caller_ ? caller_ : "WinUtil::GuardedFunctionCall") << "\n"
        << e.what()
        << std::endl;
    ExceptionBox::show();
}


auto C::callL3() -> Result
{
    auto res = Result::ok;
    try
    {
        implementCall();
    }
    catch (const d1::Exception& e)
    {
        if (not e.handled())
            showErrorBox(e);

        res = Result::exception_catched;
    }
    catch (std::exception& e)
    {
        showErrorBox(e);
    }
    return res;
}


namespace
{

void tryToRestoreGuardPageAfterStackOverflow()
{
    // see https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/resetstkoflw?view=msvc-160
    _resetstkoflw();
}

}


#pragma optimize("gt", on)
// changes global optimizer from size to speed. If size optimizer is
// used, generated code inside __except expression contains sequences like
//       push  1
//       pop   eax
// which is small (3 bytes) but uses stack, which is counterproductive
// when a stack overflow is the reason for the exception. If speed
// optimization is used, the compiler codes above sequence as
//       mov   eax,1
// which does not use stack (but is 5 bytes in size).

auto C::callL2() -> Result
{
    auto res = Result::ok;
    auto er = EXCEPTION_RECORD{};
    bool SEH_exception_catched = false;

    __try
    {
        res = callL3();
    }
    __except (
        er = *(GetExceptionInformation())->ExceptionRecord,
        EXCEPTION_EXECUTE_HANDLER)
    {
        if (er.ExceptionCode == EXCEPTION_STACK_OVERFLOW)
            tryToRestoreGuardPageAfterStackOverflow();
        SEH_exception_catched = true;
        res = Result::exception_catched;
    }

    if (SEH_exception_catched)
        SEHTranslator::throwCException(er);

    return res;
}

#pragma optimize("", off)
// turns optimization settings to previous values.


auto C::execute() throw() -> Result
{
    auto res = Result::ok;
    try
    {
        res = callL2();
    }
    catch (CException::Common& e)
    {
        res = Result::exception_catched;
        auto os = std::ostream{ &WinUtil::ExceptionBox::clear() };
        os
            << "WinUtil::CException::Common catched in "
            << (caller_ ? caller_ : "WinUtil::GuardedFunctionCall") << "\n"
            << e.what()
            << std::endl;
        ExceptionBox::show();
    }
    catch (...)
    {
        res = Result::exception_catched;
        auto os = std::ostream{ &WinUtil::ExceptionBox::clear() };
        os
            << "catch(...) exception in "
            << (caller_ ? caller_ : "WinUtil::GuardedFunctionCall") << "\n"
            << std::endl;
        ExceptionBox::show();
    }
    return res;
}

}
