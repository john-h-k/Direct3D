#pragma once

#if defined(DEBUG) != defined(_DEBUG) 
#undef DEBUG
#define DEBUG 1
#undef _DEBUG
#define _DEBUG 1
#endif

#define CONFIRM_EXIT (MessageBoxW(nullptr, L"Are you sure you want to exit?", L"Confirm exit", MB_YESNO | MB_ICONQUESTION) == IDYES)

#if UNICODE
#define MESSAGE_FAILED(HR, STRING) CheckFailW(HR, STRING)
// todo
#define MSG_FAIL_RET_FALSE(HR, STRING) if (CheckFailW(HR, STRING)) { return false; }
#define STATIC_MSG_FAIL_RET_FAIL(HR, STRING) if (renderer.CheckFailW(HR, STRING)) { Renderer renderer; renderer.initFailed = true; return renderer; }
#define MSG_FAIL_THROW(HR, STRING) if (CheckFailW(HR, STRING)) { throw std::logic_error("HRESULT failed"); }
#elif
#define MESSAGE_FAILED(HR, STRING) CheckFailA(HR, STRING)
#define MSG_FAIL_RET_FALSE(HR, STRING) (if (CheckFailA(HR, STRING)) { return false; })
#define STATIC_MSG_FAIL_RET_FAIL(HR, STRING) if (renderer.CheckFailA(HR, STRING)) { Renderer renderer; renderer.initFailed = true; return renderer; }
#define MSG_FAIL_THROW(HR, STRING) (if (CheckFailA(HR, STRING)) { throw std::logic_error("HRESULT failed"); })
#endif

#include <crtdbg.h>
#include <cstdarg>
#include <cstdio>

#ifdef _DEBUG
#define TRACEMAXSTRING  1024

#define TRACE _trace

extern char traceBuffer[TRACEMAXSTRING];
inline void _trace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int n_buf;
	n_buf = _vsnprintf_s(traceBuffer,
		TRACEMAXSTRING,
		format,
		args);
	va_end(args);

	_RPT0(_CRT_WARN, traceBuffer);
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