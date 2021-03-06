/*===--------------------------------------------------------------------------
 *                   ROCm Device Libraries
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *===------------------------------------------------------------------------*/

#include "mathH.h"

CONSTATTR static inline bool
samesign(half x, half y)
{
    return (AS_USHORT(x) & (ushort)SIGNBIT_HP16) == (AS_USHORT(y) & (ushort)SIGNBIT_HP16);
}

#if defined(COMPILING_FMOD)
CONSTATTR half
MATH_MANGLE(fmod)(half x, half y)
#elif defined(COMPILING_REMQUO)
half
MATH_MANGLE(remquo)(half x, half y, __private int *q7p)
#else
CONSTATTR half
MATH_MANGLE(remainder)(half x, half y)
#endif
{
    // How many bits of the quotient per iteration
    const int bits = 11;
    float ax = (float)BUILTIN_ABS_F16(x);
    float ay = (float)BUILTIN_ABS_F16(y);

    float ret;
#if defined(COMPILING_REMQUO)
    int q7;
#endif

    if (ax > ay) {
        int ex, ey;

        if (AMD_OPT()) {
            ex = BUILTIN_FREXP_EXP_F32(ax) - 1;
            ax = BUILTIN_FLDEXP_F32(BUILTIN_FREXP_MANT_F32(ax), bits);
            ey = BUILTIN_FREXP_EXP_F32(ay) - 1;
            ay = BUILTIN_FLDEXP_F32(BUILTIN_FREXP_MANT_F32(ay), 1);
        } else {
            ex = (AS_INT(ax) >> 23) - 127;
            ax = AS_FLOAT(((127+bits-1) << 23) | (AS_INT(ax) & 0x007fffff));
            ax = x == 0.0f ? 0.0f : ax;
            ex = x == 0.0f ? 0 : ex;
            ey = (AS_INT(ay) >> 23) - 127;
            ay = AS_FLOAT((127 << 23) | (AS_INT(ay) & 0x007fffff));
            ey = y == 0.0f ? ex : ey;
        }

        int nb = ex - ey;

        float ayinv;
        if (AMD_OPT()) {
            ayinv = BUILTIN_RCP_F32(ay);
        } else {
            ayinv = BUILTIN_DIV_F32(1.0f, ay);
        }

#if !defined(COMPILING_FMOD)
        int qacc = 0;
#endif

        while (nb > bits) {
            float q = BUILTIN_RINT_F32(ax * ayinv);
            ax = BUILTIN_MAD_F32(-q, ay, ax);
            int clt = ax < 0.0f;
            float axp = ax + ay;
            ax = clt ? axp : ax;
#if defined(COMPILING_REMQUO)
            int iq = (int)q;
            iq -= clt;
            qacc = (qacc << bits) | iq;
#endif
            if (AMD_OPT()) {
                ax = BUILTIN_FLDEXP_F32(ax, bits); 
            } else {
                ax *= AS_FLOAT((127 + bits) << 23);
            }
            nb -= bits;
        }

        if (AMD_OPT()) {
            ax = BUILTIN_FLDEXP_F32(ax, nb - bits + 1);
        } else {
            ax *= AS_FLOAT((127 + nb - bits + 1) << 23);
        }

        // Final iteration
        {
            float q = BUILTIN_RINT_F32(ax * ayinv);
            ax = BUILTIN_MAD_F32(-q, ay, ax);
            int clt = ax < 0.0f;
            float axp = ax + ay;
            ax = clt ? axp : ax;
#if !defined(COMPILING_FMOD)
            int iq = (int)q;
            iq -= clt;
#if defined(COMPILING_REMQUO)
            qacc = (qacc << (nb+1)) | iq;
#else
            qacc = iq;
#endif
#endif
        }

#if !defined(COMPILING_FMOD)
        // Adjust ax so that it is the range (-y/2, y/2]
        // We need to choose the even integer when x/y is midway between two integers
        int aq = (2.0f*ax > ay) | ((qacc & 0x1) & (2.0f*ax == ay));
        ax = ax - (aq ? ay : 0.0f);
#if defined(COMPILING_REMQUO)
        qacc += aq;
        int qneg = samesign(x, y) ? 0 : -1;
        q7 = ((qacc & 0x7f) ^ qneg) - qneg;
#endif
#endif

        if (AMD_OPT()) {
            ax = BUILTIN_FLDEXP_F32(ax, ey);
        } else {
            ax *= AS_FLOAT((127 + ey) << 23);
        }

        short ir = AS_SHORT((half)ax);
        ir ^= AS_SHORT(x) & (short)SIGNBIT_HP16;
        ret = AS_HALF(ir);
    } else {
        ret = x;
#if defined(COMPILING_REMQUO)
        q7 = 0;
#endif

#if !defined(COMPILING_FMOD)
        bool c = ax > 0.5f*ay;

        half qsgn = samesign(x,y) ? -1.0h : 1.0h;
        half t = MATH_MAD(y, qsgn, x);
        ret = c ? t : ret;
#if defined(COMPILING_REMQUO)
        q7 = c ? qsgn : q7;
#endif
#endif

        ret = ax == ay ? BUILTIN_COPYSIGN_F16(0.0h, x) : ret;
#if defined(COMPILING_REMQUO)
        q7 = ax == ay ? qsgn : q7;
#endif
    }

    if (!FINITE_ONLY_OPT()) {
        ret = y == 0.0h ? AS_HALF((short)QNANBITPATT_HP16) : ret;
#if defined(COMPILING_REMQUO)
        q7 = y == 0.0h ? 0 : q7;
#endif

        bool c = BUILTIN_CLASS_F16(y, CLASS_QNAN|CLASS_SNAN) |
                 BUILTIN_CLASS_F16(x, CLASS_NINF|CLASS_PINF|CLASS_SNAN|CLASS_QNAN);
        ret = c ? AS_HALF((short)QNANBITPATT_HP16) : ret;
#if defined(COMPILING_REMQUO)
        q7 = c ? 0 : q7;
#endif
    }

#if defined(COMPILING_REMQUO)
    *q7p = q7;
#endif
    return ret;
}

