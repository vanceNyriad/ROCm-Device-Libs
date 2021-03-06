/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathH.h"

PUREATTR INLINEATTR half
MATH_MANGLE(log2)(half x)
{
    if (AMD_OPT()) {
        return BUILTIN_LOG2_F16(x);
    } else {
        return (half)MATH_UPMANGLE(log2)((float)x);
    }
}

