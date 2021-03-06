/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathD.h"

CONSTATTR INLINEATTR double
MATH_MANGLE(logb)(double x)
{
    double ret;

    if (AMD_OPT()) {
        ret = (double)(BUILTIN_FREXP_EXP_F64(x) - 1);
    } else {
        int r = ((AS_INT2(x).hi >> 20) & 0x7ff) - EXPBIAS_DP64;
        int rs = -1011 - (int)MATH_CLZL(AS_LONG(x) & MANTBITS_DP64);
        ret = (double)(r == -EXPBIAS_DP64 ? rs : r);
    }

    if (!FINITE_ONLY_OPT()) {
        double ax = BUILTIN_ABS_F64(x);
        ret = BUILTIN_CLASS_F64(ax, CLASS_PINF|CLASS_SNAN|CLASS_QNAN) ? ax : ret;
        ret = x == 0.0 ? AS_DOUBLE(NINFBITPATT_DP64) : ret;
    }

    return ret;
}

