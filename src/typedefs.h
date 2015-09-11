#ifndef ELON_TYPEDEFS
#define ELON_TYPEDEFS

#include <cstdint>
#include <cfloat>

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


#endif
