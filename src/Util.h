#ifndef ELON_UTIL_H
#define ELON_UTIL_H

#include <cstdint>
#include <cfloat>
#include <cmath>

#define lscope static
#define glob static
#define intern static
#define scast static_cast
#define rcast reinterpret_cast

typedef int32_t B32;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN
#define F32_MIN FLT_MIN
#define F64_MIN DBL_MIN

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX
#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX
#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX
#define PI (3.1415926535897932f)
#define TAU (6.2831853071795865f)
#define E (2.71828182845904523526f)
#define RAD_TO_DEG (57.2957795f)
#define DEG_TO_RAD (0.0174532925f)
#define True (0xFFFFFFFF)
#define False (0x00000000)
#define EPSILON (0.00001f)

#define KiB(value) ((value)*1024LL)
#define MiB(value) (KiB(value)*1024LL)
#define GiB(value) (MiB(value)*1024LL)
#define TiB(value) (GiB(value)*1024LL)

#define sizeofArr(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define AlignPo2(value, alignment) ((value + ((alignment) - 1)) & ~((alignment) - 1))
#define Align4(value) ((value + 3) & ~3)
#define Align8(value) ((value + 7) & ~7)
#define Align16(value) ((value + 15) & ~15)

/**
 * Get FGPA clock time in seconds
 */
F64 SystemTime();

/**
 * Delays code execution by a given time in seconds
 */
void Sleep(F64 secs);

/**
 * Clamps a value to a max and a min
 */
F32 Clamp(F32 value, F32 min, F32 max);

/**
 * Returns max of two values
 */
F32 Max(F32 a, F32 b);

/**
 * Returns x^2
 */
F32 Sq(F32 x);

/**
 * Returns x^4
 */
F32 Qu(F32 x);

/**
 * Returns the sign of a value; -1 or +1
 */
I32 Sgn(F32 x);

/**
 * Normalizes a value of [-1.0f, 1.0f] to [0.0f, 1.0f]
 */
F32 NormalizeAlpha(F32 alpha);

/**
 * Linear Interpolation between two values
 * alpha [0.0f, 1.0f], percentage between two values
 */
F32 Lerp(F32 min, F32 max, F32 alpha);

/**
 * Cosine interpolation between two values. smoother than linear interpolation
 * alpha [0.0f, 1.0f], percentage between two values
 */
F32 Coserp(F32 min, F32 max, F32 alpha);

/**
 * Cosine interpolation between three values
 * alpha [-1.0f, 1.0f], percentage displacement from middle value
 */
F32 SystemMagnitudeInterpolation(F32 min, F32 mid, F32 max, F32 alpha);

#endif
