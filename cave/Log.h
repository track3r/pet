#pragma once

//__FUNCSIG__
void dbg_printf(const char* file, long line, const char* function, const char* format, ...);

#define LOG(...) dbg_printf(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define TRACE() dbg_printf(__FILE__, __LINE__, __FUNCTION__, "")