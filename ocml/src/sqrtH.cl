/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathH.h"

CONSTATTR INLINEATTR half
MATH_MANGLE(sqrt)(half x)
{
    return BUILTIN_SQRT_F16(x);
}

#if defined ENABLE_ROUNDED
#if defined HSAIL_BUILD

#define GEN(NAME,ROUND)\
CONSTATTR INLINEATTR half \
MATH_MANGLE(NAME)(half x) \
{ \
    return BUILTIN_FULL_UNARY(fsqrth, false, ROUND, x); \
}

GEN(sqrt_rte, ROUND_TO_NEAREST_EVEN)
GEN(sqrt_rtp, ROUND_TO_POSINF)
GEN(sqrt_rtn, ROUND_TO_NEGINF)
GEN(sqrt_rtz, ROUND_TO_ZERO)

#endif // HSAIL_BUILD
#endif // ENABLE_ROUNDED

