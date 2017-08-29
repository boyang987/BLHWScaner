
#include <cstdio>

#include <Windows.h>

/// @brief 最大处理器物理核心数
#ifndef MAX_PROCESSOR_PHYSICAL_CORE_NUM
#define MAX_PROCESSOR_PHYSICAL_CORE_NUM 64
#endif

/// @brief 最大GPU温度传感器数量
#ifndef MAX_GPU_SENSORS_NUMBER
#define MAX_GPU_SENSORS_NUMBER 16
#endif

typedef bool (*GetCpuTempFun)(OUT unsigned int* pCoreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM]);

typedef bool (*GetGpuTempFun)(OUT unsigned int* pSensorNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER]);

int main()
{
    bool bRet = false;

    unsigned int cpuCoreNum = 0;
    unsigned int gpuCoreNum = 0;
    unsigned int cpuTemp[MAX_PROCESSOR_PHYSICAL_CORE_NUM] = {0};
    unsigned int gpuTemp[MAX_GPU_SENSORS_NUMBER] = {0};

    HMODULE hCoreTemp = NULL; // CoreTemp DLL句柄
    GetCpuTempFun pGetCpuTemp = NULL; // 获取CPU温度函数指针
    GetGpuTempFun pGetGpuTemp = NULL; // 获取GPU温度函数指针

    hCoreTemp = LoadLibraryW(L".\\CoreTemp.dll");
    if (hCoreTemp == NULL)
    {
        printf("CoreTemp.dll Can't Load\n");
        goto SAFE_EXIT;
    }

    pGetCpuTemp = (GetCpuTempFun)GetProcAddress(hCoreTemp, "GetCpuTemp");
    pGetGpuTemp = (GetGpuTempFun)GetProcAddress(hCoreTemp, "GetGpuTemp");
    if (pGetCpuTemp == NULL ||
        pGetGpuTemp == NULL)
    {
        printf("Get CoreTemp Function Fail\n");
        goto SAFE_EXIT;
    }

    bRet = pGetCpuTemp(&cpuCoreNum, cpuTemp);
    if (!bRet)
    {
        printf("GetCpuTemp Fail\n");
    }
    else
    {
        printf("Cpu Core Num Is: %u\n", cpuCoreNum);
        for (unsigned int i = 0; i < cpuCoreNum; i++)
        {
            printf("Cpu Core(%u) Temp Is: %u\n", i, cpuTemp[i]);
        }
    }

    printf("\n");

    bRet = pGetGpuTemp(&gpuCoreNum, gpuTemp);
    if (!bRet)
    {
        printf("GetGpuTemp Fail\n");
    }
    else
    {
        printf("Gpu Core Num Is: %u\n", gpuCoreNum);
        for (unsigned int i = 0; i < gpuCoreNum; i++)
        {
            printf("Gpu Core(%u) Temp Is: %u\n", i, gpuTemp[i]);
        }
    }


SAFE_EXIT:
    system("pause");

    if (hCoreTemp != NULL)
    {
        FreeLibrary(hCoreTemp);
        hCoreTemp = NULL;
    }

    return 0;
}