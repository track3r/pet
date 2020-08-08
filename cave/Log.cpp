#include "pch.h"

#include "Log.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WIN32_EXTRA_LEAN
#define WIN32_EXTRA_LEAN
#endif

#define NOIME
#define NOWINRES
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOUSER
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME
#define NOPROXYSTUB
#define NOIMAGE
#define NO
#define NOTAPE
#define ANSI_ONLY

#include <Windows.h>
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
	szBuffer[0] = 0;
	int64_t ticks = GetTickCount64() - g_startTick;
	//snprintf(szBuffer, 512, "%s( %li ): [%s][%d-%d]%llu %llu\t",file, line, function, GetCurrentThreadId(), GetCurrentProcessorNumber(), ticks / 1000, ticks % 1000);
	if (line != 0)
	{
		if (function[0] == 0)
		{
			snprintf(szBuffer, 512, "%s(%li): ", file, line);
		}
		else
		{
			snprintf(szBuffer, 512, "%s(%li): %s() ", file, line, function);
		}
	}
	else
	{
		if (function[0] != 0)
		{
			snprintf(szBuffer, 512, "%s ", function);
		}		
	}
    
	if (szBuffer[0] != 0)
	{
		::OutputDebugStringA(szBuffer);
		printf("%s", (szBuffer));
		//log(szBuffer);
	}
    
	nBuf = _vsnprintf(szBuffer, 511, format, args);
	::OutputDebugStringA(szBuffer);
	printf("%s", (szBuffer));

    snprintf(szBuffer, 512, " [%llu.%llu %d-%d]\n", ticks / 1000, ticks % 1000, GetCurrentThreadId(), GetCurrentProcessorNumber());
	dbg_puts(szBuffer);
	//log(szBuffer);
	va_end(args);
}

void dbg_puts(const char* str)
{
	::OutputDebugStringA(str);
	printf("%s", (str));
}