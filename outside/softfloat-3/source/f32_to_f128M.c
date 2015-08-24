
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014 The Regents of the University of California
(Regents).  All Rights Reserved.  Redistribution and use in source and binary
forms, with or without modification, are permitted provided that the following
conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions, and the following two paragraphs of disclaimer.
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions, and the following two paragraphs of disclaimer in the
documentation and/or other materials provided with the distribution.  Neither
the name of the Regents nor the names of its contributors may be used to
endorse or promote products derived from this software without specific prior
written permission.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
HEREUNDER IS PROVIDED "AS IS".  REGENTS HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

#ifdef SOFTFLOAT_FAST_INT64

void f32_to_f128M( float32_t a, float128_t *zPtr )
{

    *zPtr = f32_to_f128( a );

}

#else

void f32_to_f128M( float32_t a, float128_t *zPtr )
{
    uint32_t *zWPtr;
    union ui32_f32 uA;
    uint32_t uiA;
    bool sign;
    int_fast16_t exp;
    uint32_t sig, uiZ64;
    struct commonNaN commonNaN;
    uint32_t uiZ96;
    struct exp16_sig32 normExpSig;
    uint64_t sig64;

    zWPtr = (uint32_t *) zPtr;
    uA.f = a;
    uiA = uA.ui;
    sign = signF32UI( uiA );
    exp  = expF32UI( uiA );
    sig  = fracF32UI( uiA );
    uiZ64 = 0;
    if ( exp == 0xFF ) {
        if ( sig ) {
            softfloat_f32UIToCommonNaN( uiA, &commonNaN );
            softfloat_commonNaNToF128M( &commonNaN, zWPtr );
            return;
        }
        uiZ96 = packToF128UI96( sign, 0x7FFF, 0 );
        goto uiZ;
    }
    if ( ! exp ) {
        if ( ! sig ) {
            uiZ96 = packToF128UI96( sign, 0, 0 );
            goto uiZ;
        }
        normExpSig = softfloat_normSubnormalF32Sig( sig );
        exp = normExpSig.exp - 1;
        sig = normExpSig.sig;
    }
    sig64 = (uint64_t) sig<<25;
    uiZ96 = packToF128UI96( sign, exp + 0x3F80, sig64>>32 );
    uiZ64 = sig64;
 uiZ:
    zWPtr[indexWord( 4, 3 )] = uiZ96;
    zWPtr[indexWord( 4, 2 )] = uiZ64;
    zWPtr[indexWord( 4, 1 )] = 0;
    zWPtr[indexWord( 4, 0 )] = 0;

}

#endif
