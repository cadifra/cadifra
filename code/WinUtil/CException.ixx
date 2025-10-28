/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.CException;

import std;


export namespace WinUtil::CException
{

class Common: public std::exception
{
protected:
    const unsigned int itsFlags = 0;
    const void* itsAddress = nullptr; // ref only
    mutable std::string itsBuffer;

public:
    Common();
    Common(const EXCEPTION_RECORD&);

    virtual const char* what() const;

    Common(const Common&) = default;
    Common& operator=(const Common&) = default;

private:
    virtual const char* GetName() const = 0;
};


#define D1_DEFINE_CEXCEPTION_CLASS(name, parent)     \
    class name: public parent                        \
    {                                                \
        virtual const char* GetName() const          \
        {                                            \
            return #name;                            \
        }                                            \
                                                     \
    public:                                          \
        name(const EXCEPTION_RECORD& er): parent(er) \
        {                                            \
        }                                            \
    };


D1_DEFINE_CEXCEPTION_CLASS(Unknown, Common)
// D1_DEFINE_CEXCEPTION_CLASS(Access_violation,         Common)
D1_DEFINE_CEXCEPTION_CLASS(Datatype_misalignment, Common)
D1_DEFINE_CEXCEPTION_CLASS(Array_bounds_exceeded, Common)
D1_DEFINE_CEXCEPTION_CLASS(Priv_instruction, Common)
D1_DEFINE_CEXCEPTION_CLASS(In_page_error, Common)
D1_DEFINE_CEXCEPTION_CLASS(Illegal_instruction, Common)
D1_DEFINE_CEXCEPTION_CLASS(Noncontinuable_exception, Common)
D1_DEFINE_CEXCEPTION_CLASS(Stack_overflow, Common)
D1_DEFINE_CEXCEPTION_CLASS(Invalid_disposition, Common)
D1_DEFINE_CEXCEPTION_CLASS(Guard_page, Common)
D1_DEFINE_CEXCEPTION_CLASS(Invalid_handle, Common)


D1_DEFINE_CEXCEPTION_CLASS(Integer, Common)
D1_DEFINE_CEXCEPTION_CLASS(Int_divide_by_zero, Integer)
D1_DEFINE_CEXCEPTION_CLASS(Int_overflow, Integer)

D1_DEFINE_CEXCEPTION_CLASS(FloatingPoint, Common)
D1_DEFINE_CEXCEPTION_CLASS(Flt_denormal_operand, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_divide_by_zero, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_inexact_result, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_invalid_operation, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_overflow, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_stack_check, FloatingPoint)
D1_DEFINE_CEXCEPTION_CLASS(Flt_underflow, FloatingPoint)


D1_DEFINE_CEXCEPTION_CLASS(Breakpoint, Common)
D1_DEFINE_CEXCEPTION_CLASS(Single_step, Common)
D1_DEFINE_CEXCEPTION_CLASS(Control_c_exit, Common)



class Access_violation: public Common
{
    const bool itWasAWrite{};
    const void* itsAccessAddress = nullptr;
    virtual const char* GetName() const { return "Access_violation"; }

public:
    Access_violation(const EXCEPTION_RECORD& er);
    virtual const char* what() const;
};

}
