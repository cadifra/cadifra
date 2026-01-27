/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.SEHTranslator;

import WinUtil.CException;


namespace WinUtil::SEHTranslator
{

void throwCException(const EXCEPTION_RECORD& er)
{
    DWORD code = er.ExceptionCode;

    using namespace CException;

    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        throw Access_violation{ er };
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        throw Datatype_misalignment{ er };
        break;
    case EXCEPTION_BREAKPOINT:
        throw Breakpoint{ er };
        break;
    case EXCEPTION_SINGLE_STEP:
        throw Single_step{ er };
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        throw Array_bounds_exceeded{ er };
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        throw Flt_denormal_operand{ er };
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        throw Flt_divide_by_zero{ er };
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        throw Flt_inexact_result{ er };
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        throw Flt_invalid_operation{ er };
        break;
    case EXCEPTION_FLT_OVERFLOW:
        throw Flt_overflow{ er };
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        throw Flt_stack_check{ er };
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        throw Flt_underflow{ er };
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        throw Int_divide_by_zero{ er };
        break;
    case EXCEPTION_INT_OVERFLOW:
        throw Int_overflow{ er };
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        throw Priv_instruction{ er };
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        throw In_page_error{ er };
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        throw Illegal_instruction{ er };
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        throw Noncontinuable_exception{ er };
        break;
    case EXCEPTION_STACK_OVERFLOW:
        throw Stack_overflow{ er };
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        throw Invalid_disposition{ er };
        break;
    case EXCEPTION_GUARD_PAGE:
        throw Guard_page{ er };
        break;
    case EXCEPTION_INVALID_HANDLE:
        throw Invalid_handle{ er };
        break;
    case CONTROL_C_EXIT:
        throw Control_c_exit{ er };
        break;
    default:
        throw Unknown{ er };
    }
}

}
