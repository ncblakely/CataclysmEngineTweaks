/*=============================================================================
    Name    : crc32.h
    Purpose : Compute CRC32 for packets of data

    Created 6/29/1997 by lmoloney
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#ifndef ___CRC32_H
#define ___CRC32_H

#include "types.h"

/*=============================================================================
    Type definitions
=============================================================================*/
typedef udword crc32;
typedef uword crc16;

/*=============================================================================
    Functions
=============================================================================*/
crc32 crc32Compute(ubyte *packet, udword length);
crc16 crc16Compute(ubyte *packet, udword length);

#endif //___CRC32_H


