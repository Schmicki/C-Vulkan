#pragma once



#include <stddef.h> /* ptrdiff_t, size_t, ... */
#include <stdint.h> /* int8_t, int16_t, ... */
#include <limits.h> /* INT_MAX, UINT_MAX, ... */
#include <assert.h>
#include <stdlib.h> /* malloc, free, ... */
#include <string.h> /* memcpy, memmove, ... */
#include <time.h>



/**************************************************************************************************/
/*	Detect Compiler  */

#if defined(_MSC_VER)
#define COMPILER_MSVC 1

#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#define COMPILER_GCC 1

#elif defined(__clang__)
#define COMPILER_CLANG 1

#else
#error detection could not find compiler, please implement it

#endif

/*	Detect Architecture */

#if defined(_M_AMD64) || defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define PLATFORM_X64 1

#elif defined(_M_IX86) || defined(__i386__)
#define PLATFORM_X86 1

#elif  defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM64EC)
#define PLATFORM_ARM64 1

#elif defined(_M_ARM) || defined(__ar_)
#define PLATFORM_ARM32 1

#elif defined(__mips__)
#define PLATFORM_MIPS 1

#else
#error detection could not find architecture, please implement it

#endif

/*	Detect OS */

#if defined(_WIN64) || defined(_WIN32) || defined(WIN32)
#define OS_WINDOWS 1

#elif defined(__ANDROID__) || defined(ANDROID)
#define OS_ANDROID 1

#elif defined(__linux__)
#define OS_LINUX 1

#elif defined(__APPLE__)
#define OS_MACOS 1

#elif defined(__PSP__)
#define OS_PSP 1

#else
#error detection could not find system, please implement it

#endif

/*	Detect Build Mode */

#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG) || BUILD_DEBUG
#ifndef BUILD_DEBUG
#define BUILD_DEBUG 1
#endif

#else
#ifndef BUILD_RELEASE
#define BUILD_RELEASE 1
#endif

#ifndef NDEBUG
#define NDEBUG 1
#endif

#endif

/*	Detect Platform Features */

#if defined(PLATFORM_X86)
#define PLATFORM_HAS_F64 1
#define CACHE_LINE 64

#endif /* X86 */

#if defined(PLATFORM_X64)
#define PLATFORM_HAS_I64 1
#define PLATFORM_HAS_F64 1
#define CACHE_LINE 64

#endif /* X64 */

#if defined(PLATFORM_ARM32)
#define CACHE_LINE 64

#endif /* ARM32 */

#if defined(PLATFORM_ARM64)
#define PLATFORM_HAS_I64 1
#define CACHE_LINE 64

#endif /* ARM64 */

#if defined(PLATFORM_MIPS)
#define CACHE_LINE 64

#endif /* MIPS */



/**************************************************************************************************/
/*	Intrinsics  */

#if defined(COMPILER_MSVC) /* MSVC */
#define libexport __declspec(dllexport)
#define libimport __declspec(dllimport)
#define libhide
#define forceinline __forceinline

#define alignas(alignment) __declspec(align(alignment))
#define alignof(T) __alignof(T)

#endif /* MSVC */

#if defined(COMPILER_GCC) /* GCC */
#define libexport __attribute__((visibility("default")))
#define libimport
#define libhide __attribute__((visibility("hidden")))
#define forceinline __attribute__((always_inline))

#define alignas(alignment) __attribute__ ((aligned (alignment)))
#define alignof(T) __alignof__(T)

#endif /* GCC */

#if defined(COMPILER_CLANG) /* CLANG */
#define libexport __attribute__((visibility("default")))
#define libimport
#define libhide __attribute__((visibility("hidden")))
#define forceinline __attribute__((always_inline))

#define alignas(alignment) __attribute__ ((aligned (alignment)))
#define alignof(T) __alignof__(T)

#endif /* CLANG */

#define cachealign alignas(CACHE_LINE)



/**************************************************************************************************/
/*	Types  */

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#if defined(PLATFORM_HAS_I64)
typedef int64_t s64;
typedef uint64_t u64;

#endif /* I64 */

typedef int				sint;
typedef unsigned int	uint;
typedef ptrdiff_t		sptr;
typedef size_t			uptr;

#if defined(FLT_64)
typedef double			flt;

#else
typedef float			flt;

#endif /* FLT_64 */

#define S8_MIN -128
#define S8_MAX 127
#define U8_MAX 255

#define S16_MIN -32768
#define S16_MAX 32767
#define U16_MAX 65535

#define S32_MIN -2147483648
#define S32_MAX 2147483647
#define U32_MAX 4294967295

#if defined(PLATFORM_HAS_I64)
#define S64_MIN -0x8000000000000000
#define S64_MAX 0x7FFFFFFFFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFF

#endif /* I64 */

#if defined(FLT_64)
#define FLT_EPSILON 0.000000000000001
#define FLT_EPSILON 1E-9f
#define FLT_MAX	1E+37f
#define FLT_MIN	1E-37f

#else
#define FLT_EPSILON 1E-5f
#define FLT_MAX	1E+37f
#define FLT_MIN	1E-37f

#endif /* FLT_64 */



/**************************************************************************************************/
/*	Errors  */

void throw_exception(const char* format, ...);

#define SUCCESS 0
#define INVALID_INDEX -1



/**************************************************************************************************/
/*	Memory  */

sint clz(uint val);
sint ctz(uint val);


