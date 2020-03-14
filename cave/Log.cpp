#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <Windows.h>
#include "Log.h"

//int64_t GetTickCount64();
//__declspec(dllimport) unsigned __int64 __stdcall GetTickCount64();
//__declspec(dllimport) uint32_t __stdcall GetCurrentThreadId();
//__declspec(dllimport) uint32_t __stdcall GetCurrentProcessorNumber();
//__declspec(dllimport) void __stdcall OutputDebugStringA(const char* str);

static uint64_t g_startTick = GetTickCount64();
void dbg_printf(const char* file, long line, const char* function, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int nBuf;
	char szBuffer[512];

	int64_t ticks = GetTickCount64() - g_startTick;
	//snprintf(szBuffer, 512, "%s( %li ): [%s][%d-%d]%llu %llu\t",file, line, function, GetCurrentThreadId(), GetCurrentProcessorNumber(), ticks / 1000, ticks % 1000);
    snprintf(szBuffer, 512, "%s( %li ): %s ", file, line, function);
	::OutputDebugStringA(szBuffer);
    printf("%s", (szBuffer));
	//log(szBuffer);
    
	nBuf = _vsnprintf(szBuffer, 511, format, args);
	::OutputDebugStringA(szBuffer);
	printf("%s", (szBuffer));

    snprintf(szBuffer, 512, " [%llu.%llu %d-%d]\n", ticks / 1000, ticks % 1000, GetCurrentThreadId(), GetCurrentProcessorNumber());
    ::OutputDebugStringA(szBuffer);
    printf("%s", (szBuffer));
	//log(szBuffer);
	va_end(args);
}