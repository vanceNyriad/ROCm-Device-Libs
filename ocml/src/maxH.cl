/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathH.h"

CONSTATTR INLINEATTR half
MATH_MANGLE(max)(half x, half y)
{
    if (AMD_OPT()) {
        return BUILTIN_CMAX_F16(x, y);
    } else {
        return BUILTIN_MAX_F16(x, y);
    }
}

