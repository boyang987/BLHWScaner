
#include "LMemory.h"

#include <Windows.h>

LMemory::LMemory()
{

}

LMemory::~LMemory()
{

}

int LMemory::GetLoadPercent()
{
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    BOOL iRet = GlobalMemoryStatusEx(&memoryStatus);
    if (iRet == FALSE)
        return -1;

    return memoryStatus.dwMemoryLoad;
}

int LMemory::GetTotalSize()
{
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    BOOL iRet = GlobalMemoryStatusEx(&memoryStatus);
    if (iRet == FALSE)
        return -1;

    DWORDLONG total64 = memoryStatus.ullTotalPhys;
    total64 = total64/1024/1024;

    int total = (int)total64;

    return total;
}

int LMemory::GetAvailableSize()
{
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    BOOL iRet = GlobalMemoryStatusEx(&memoryStatus);
    if (iRet == FALSE)
        return -1;

    DWORDLONG avail64 = memoryStatus.ullAvailPhys;
    avail64 = avail64/1024/1024;

    int avail = (int)avail64;

    return avail;
}