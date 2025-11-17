#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

#include "d1/d1assert.h"



#define D1_CHECK_PTR(p)     \
    D1_ASSERT((p) != 0);    \
    D1_ASSERT(!(p)->IsInTrash());
