#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */


#include "d1assert.h"

#ifdef _DEBUG
#define D1_VERIFY(expr) D1_PERMANENT_ASSERT(expr)
#else
#define D1_VERIFY(expr) if (expr) { }
#endif
