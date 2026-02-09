#include "core.h"

#if defined(COMPILER_MSVC)
#include <intrin.h>
#elif defined(COMPILER_GCC)

#elif defined(COMPILER_CLANG)

#endif

#include <stdarg.h>
#include <stdio.h>



/**************************************************************************************************/
/*	Memory  */

sint clz(uint val)
{
#if defined(COMPILER_MSVC)
	unsigned long index;
	_BitScanReverse(&index, val);
	return 31 - index;
	
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
	return __builtin_clz(val);
	
#else
	int_t result = 0;

	if ((val & 0xFFFF0000) == 0)
	{
		result += 16;
		val <<= 16;
	}

	if ((val & 0xFF000000) == 0)
	{
		result += 8;
		val <<= 8;
	}

	if ((val & 0xF0000000) == 0)
	{
		result += 4;
		val <<= 4;
	}

	if ((val & 0xC0000000) == 0)
	{
		result += 2;
		val <<= 2;
	}

	result += ((val & 0x80000000) == 0);
	return result;

#endif
}

sint ctz(uint val)
{
#if defined(COMPILER_MSVC)
	unsigned long index;
	_BitScanForward(&index, val);
	return index;

#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
	return __builtin_ctz(val);

#else
	int_t result = 0;

	if ((val & 0xFFFF) == 0)
	{
		result += 16;
		val >>= 16;
	}

	if ((val & 0xFF) == 0)
	{
		result += 8;
		val >>= 8;
	}

	if ((val & 0xF) == 0)
	{
		result += 4;
		val >>= 4;
	}

	if ((val & 0x3) == 0)
	{
		result += 2;
		val >>= 2;
	}

	result += ((~val) & 0x1);
	return result;
#endif
}



/**************************************************************************************************/
/*	Error  */

void throw_exception(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	fflush(stdout);
	abort();
}


