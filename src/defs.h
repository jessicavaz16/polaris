#pragma once
#include <stdint.h>

typedef uint32_t xlen_t; // Define xlen_t as a 32-bit unsigned integer

// generate a mask for a bit field
#define GEN_MASK(width, start) ((1 << (width)) - 1) << (start)

// define a macro to access a bit field
// #define BIT_FIELD(value, end, start) (eliminate bits after end) & (eliminate bits before start by generating a mask)
#define BIT_FIELD(value, end, start) \
    (((value) >> (start)) & ((1 << ((end) - (start) + 1)) - 1))

// #define BIT_FIELD(value, end, start) (eliminate bits after end) & (eliminate bits before start by generating a mask) | (check if sign bit is set and generaate a mask for the sign extension)
#define BIT_FIELD_SIGNED(value, end, start) \
    (((value) >> (start)) & ((1 << ((end) - (start) + 1)) - 1)) \
    | ((((value) >> (end)) & 1) ? ~((1 << ((end) - (start) + 1)) - 1) : 0)
