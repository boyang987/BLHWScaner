#ifndef _DEBUGPRINT_H_
#define _DEBUGPRINT_H_

#include <cstdio>

#ifdef _DEBUG
#define DebugPrint(format, ...) printf(format, __VA_ARGS__)
#else
#define DebugPrint(format, ...)
#endif

#endif