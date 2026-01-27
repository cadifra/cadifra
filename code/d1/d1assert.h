#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */


#pragma warning(error: 4706) // assignment within conditional expression
//
//  This should catch D1_ASSERT(a = b), which is almost an error.
//  So let's treat it as an error.


// call the debugger, if it is running
#define D1_PERMANENT_ASSERT(expr)  if (not (expr)) { __debugbreak(); }


#ifdef _DEBUG
#define D1_ASSERT(expr) D1_PERMANENT_ASSERT(expr)
#else
#define D1_ASSERT(expr)
#endif
