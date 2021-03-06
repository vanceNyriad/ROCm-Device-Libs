/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathH.h"

PUREATTR INLINEATTR half
MATH_MANGLE(exp2)(half x)
{
    if (AMD_OPT()) {
        return BUILTIN_EXP2_F16(x);
    } else {
        return (half)MATH_UPMANGLE(exp2)((float)x);
    }
}

