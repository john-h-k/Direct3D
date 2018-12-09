#pragma once

#if UNICODE
#define MESSAGE_FAILED(HR, STRING) CheckFailW(HR, STRING)
#elif
#define MESSAGE_FAILED(HR, STRING) CheckFailA(HR, STRING)
#endif

#include <crtdbg.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#define TRACEMAXSTRING  1024

#define TRACE _trace

extern char szBuffer[TRACEMAXSTRING];
inline void _trace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int nBuf;
	nBuf = _vsnprintf_s(szBuffer,
		TRACEMAXSTRING,
		format,
		args);
	va_end(args);

	_RPT0(_CRT_WARN, szBuffer);
}
#define TRACEF _snprintf(szBuffer,TRACEMAXSTRING,"%s(%d): ", \
	                &strrchr(__FILE__,'\\')[1],__LINE__); \
	                _RPT0(_CRT_WARN,szBuffer); \
	                TRACE
#else
// Remove for release mode
#define TRACE  ((void)0)
#define TRACEF ((void)0)
#endif