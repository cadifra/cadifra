/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <float.h>
#include <math.h>

module d1.isEqual;


namespace
{
constexpr int inexact_exp = DBL_MANT_DIG - 8;
}


namespace d1
{

bool isEqual(const float64& f1, const float64& f2)
{
    float64 epsilon = 0.0;

    int exp = 0;

    if (f1 == 0.0)
        frexp(f2, &exp);
    else
        frexp(f1, &exp);

    if (exp > DBL_MIN_EXP + inexact_exp)
        epsilon = ldexp(1.0, exp - inexact_exp);
    else
        epsilon = ldexp(1.0, DBL_MIN_EXP + inexact_exp);

    return (f1 < f2 + epsilon) && (f2 < f1 + epsilon);
}


}
