
#include "LGpuTemp.h"
#include <Windows.h>

#include "DebugPrint.h"

#include ".\\nvapi\\nvapi.h"
#include ".\\adlapi\\adl_sdk.h"

#pragma comment(lib, ".\\nvapi\\nvapi.lib")


/*
* 该文件中声明了CGpuTemp, CNvGpuTemp, CAMDGpuTemp3个类, 
* CNvGpuTemp类的实现是通过使用英伟达开发包nvapi来实现的
* CAMDGpuTemp类的实现是通过使用AMD开发包ADL(AMD Display Library)来实现的
*/

/// <SUMMARY>
/// GpuTemp基类
/// </SUMMARY>
class CGpuTemp
{
public:
    /// @brief 构造函数
    CGpuTemp(){}

    /// @brief 析构函数
    virtual ~CGpuTemp(){}

    /// <SUMMARY>
    /// 获取温度
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功返回true, 失败返回false
    /// </RETURNS>
    virtual bool GetTemp(OUT unsigned int& sensorsNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER]){return false;}
};

/// <SUMMARY>
/// NVIDIA(英伟达)GpuTemp类
/// </SUMMARY>
class LNvGpuTemp : public CGpuTemp
{
public:
    /// @brief 构造函数
    LNvGpuTemp()
        : INVALID_GPU_INDEX(-1)
    {
        DebugPrint("LNvGpuTemp()\n");

        m_usingGpuIndex = INVALID_GPU_INDEX;

        NvAPI_Status nvRet = NvAPI_Initialize();
        if (nvRet != NVAPI_OK)
            return;

        NvU32 gpuCount = 0;
        nvRet = NvAPI_EnumPhysicalGPUs(m_hNvPhysicalGpuList, &gpuCount);
        if (nvRet != NVAPI_OK || gpuCount == 0)
            return;


        m_usingGpuIndex = 0;
    }
    ~LNvGpuTemp()
    {
        ;
    }

    virtual bool GetTemp(OUT unsigned int& sensorsNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER])
    {
        if (m_usingGpuIndex == INVALID_GPU_INDEX)
            return false;

        /* 获取温度前先获取其他信息, 不然获取温度会失败*/
        NvU32 busId = 0;
        NvAPI_GPU_GetBusId(m_hNvPhysicalGpuList[m_usingGpuIndex], &busId);

        NV_GPU_THERMAL_SETTINGS thermal;
        thermal.version = NV_GPU_THERMAL_SETTINGS_VER;  
        NvAPI_Status nvRet = NvAPI_GPU_GetThermalSettings(m_hNvPhysicalGpuList[m_usingGpuIndex], NVAPI_THERMAL_TARGET_ALL, &thermal);
        if (nvRet != NVAPI_OK)
            return false;

        sensorsNum = thermal.count;
        for (unsigned int i = 0; i < thermal.count; i++)
        {
            temp[i] = thermal.sensor[i].currentTemp;
        }

        return true;
    }

private:
    int m_usingGpuIndex; // 正在使用的Gpu句柄索引
    const int INVALID_GPU_INDEX; // 无效Gpu句柄索引
    NvPhysicalGpuHandle m_hNvPhysicalGpuList[NVAPI_MAX_PHYSICAL_GPUS]; // 物理Gpu句柄表
};


/// <SUMMARY>
/// 初始化ADL2接口
/// </SUMMARY>
/// <PARAM name = "callback" dir = "IN">
/// 内存分配回调函数
/// </PARAM>
/// <PARAM name = "iEnumConnectedAdapters" dir = "IN">
/// 设为1得到物理存在并且被启用的显卡, 设为0得到所有曾经存在的显卡
/// </PARAM>
/// <PARAM name = "context" dir = "OUT">
/// 存储ADL2环境句柄
/// </PARAM>
/// <RETURNS>
/// 成功返回ADL_OK, 失败返回错误码
/// </RETURNS>
typedef int (*ADL2_MAIN_CONTROL_CREATE )(IN ADL_MAIN_MALLOC_CALLBACK callback, IN int iEnumConnectedAdapters, OUT ADL_CONTEXT_HANDLE* context);

/// <SUMMARY>
/// 卸载ADL2
/// </SUMMARY>
/// <PARAM name = "context" dir = "IN">
/// ADL2环境句柄
/// </PARAM>
/// <RETURNS>
/// 成功返回ADL_OK, 失败返回错误码
/// </RETURNS>
typedef int (*ADL2_MAIN_CONTROL_DESTROY )(IN ADL_CONTEXT_HANDLE context);

/// <SUMMARY>
/// 获取适配器数量
/// </SUMMARY>
/// <PARAM name = "context" dir = "IN">
/// ADL2环境句柄
/// </PARAM>
/// <PARAM name = "lpNumAdapters" dir = "OUT">
/// 存储适配器数量
/// </PARAM>
/// <RETURNS>
/// 成功返回ADL_OK, 失败返回错误码
/// </RETURNS>
typedef int (*ADL2_ADAPTER_NUMBEROFADAPTERS_GET ) (IN ADL_CONTEXT_HANDLE context, OUT int* lpNumAdapters);

/// <SUMMARY>
/// 获取适配器信息
/// </SUMMARY>
/// <PARAM name = "context" dir = "IN">
/// ADL2环境句柄
/// </PARAM>
/// <PARAM name = "lpInfo" dir = "OUT">
/// 存储适配器信息
/// </PARAM>
/// <PARAM name = "iInputSize" dir = "IN">
/// 输入缓冲区的大小
/// </PARAM>
/// <RETURNS>
/// 成功返回ADL_OK, 失败返回错误码
/// </RETURNS>
typedef int (*ADL2_ADAPTER_ADAPTERINFO_GET)(IN ADL_CONTEXT_HANDLE context, OUT LPAdapterInfo lpInfo, IN int iInputSize);

/// <SUMMARY>
/// 获取显卡温度
/// </SUMMARY>
/// <PARAM name = "context" dir = "IN">
/// ADL2环境句柄
/// </PARAM>
/// <PARAM name = "iAdapterIndex" dir = "IN">
/// 适配器索引
/// </PARAM>
/// <PARAM name = "iThermalControllerIndex" dir = "IN">
/// 热传感器索引
/// </PARAM>
/// <PARAM name = "lpTemperature" dir = "OUT">
/// 存储温度
/// </PARAM>
/// <RETURNS>
/// 成功返回ADL_OK, 失败返回错误码
/// </RETURNS>
typedef int (*ADL2_OVERDRIVE5_TEMPERATURE_GET)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iThermalControllerIndex, ADLTemperature* lpTemperature);

void* __stdcall ADL_Main_Memory_Alloc ( int iSize )

{
    void* lpBuffer = malloc ( iSize );
    return lpBuffer;
}

void __stdcall ADL_Main_Memory_Free ( void** lpBuffer )
{
    if ( NULL != *lpBuffer )
    {
        free ( *lpBuffer );
        *lpBuffer = NULL;
    }
}

/// <SUMMARY>
/// AMD GpuTemp类
/// </SUMMARY>
class LAMDGpuTemp : public CGpuTemp
{
public:
    /// @brief 构造函数
    LAMDGpuTemp()
        : INVALID_ADAPTER_INDEX(-1)
    {
        DebugPrint("LAMDGpuTemp()\n");

        m_hADLContext = NULL;
        ADL2_Main_Control_Create = NULL;
        ADL2_Main_Control_Destroy = NULL;
        ADL2_Adapter_AdapterInfo_Get = NULL;
        ADL2_Adapter_AdapterInfo_Get = NULL;
        m_hADLDll = NULL;
        m_usingAdapterIndex = INVALID_ADAPTER_INDEX;

        // 安装了AMD显卡驱动的系统目录System32下存在atiadlxx.dll(32位或64位)
        // 在64位系统上, 32位的程序装载64位的dll会失败, 所以需要在装载SysWOW64目录下的atiadlxy.dll
        m_hADLDll = LoadLibraryA("atiadlxx.dll");
        if (m_hADLDll == NULL)
            m_hADLDll = LoadLibraryA("atiadlxy.dll");
        if (m_hADLDll == NULL)
        {
            DebugPrint("Can't Find atiadlxx.dll or atiadlxy.dll\n");
            return;
        }

        ADL2_Main_Control_Create = (ADL2_MAIN_CONTROL_CREATE)GetProcAddress(m_hADLDll, "ADL2_Main_Control_Create");
        ADL2_Main_Control_Destroy = (ADL2_MAIN_CONTROL_DESTROY)GetProcAddress(m_hADLDll, "ADL2_Main_Control_Destroy");
        ADL2_Adapter_NumberOfAdapters_Get = (ADL2_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(m_hADLDll, "ADL2_Adapter_NumberOfAdapters_Get");
        ADL2_Adapter_AdapterInfo_Get = (ADL2_ADAPTER_ADAPTERINFO_GET)GetProcAddress(m_hADLDll, "ADL2_Adapter_AdapterInfo_Get");
        ADL2_Overdrive5_Temperature_Get = (ADL2_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(m_hADLDll, "ADL2_Overdrive5_Temperature_Get");

        if (ADL2_Main_Control_Create == NULL ||
            ADL2_Main_Control_Destroy == NULL ||
            ADL2_Adapter_NumberOfAdapters_Get == NULL ||
            ADL2_Adapter_AdapterInfo_Get == NULL ||
            ADL2_Overdrive5_Temperature_Get == NULL)
        {
            DebugPrint("Can't Get AMD Function Address\n");
            return;
        }

        int iRet;

        iRet = ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 1, &m_hADLContext);
        if (iRet != ADL_OK)
            return;

        int adapterCount = 0;
        iRet = ADL2_Adapter_NumberOfAdapters_Get(m_hADLContext, &adapterCount);
        if (iRet != ADL_OK)
            return;

        if (adapterCount < 1)
            return;


        m_usingAdapterIndex = 0;
    }

    /// @brief 析构函数
    ~LAMDGpuTemp()
    {
        if (ADL2_Main_Control_Destroy != NULL)
        {
            ADL2_Main_Control_Destroy(m_hADLContext);
        }

        if (m_hADLDll != NULL)
        {
            FreeLibrary(m_hADLDll);
            m_hADLDll = NULL;
        }
    }

    virtual bool GetTemp(OUT unsigned int& sensorsNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER])
    {
        if (m_usingAdapterIndex == INVALID_ADAPTER_INDEX)
            return false;

        ADLTemperature adlTemp;
        if (ADL2_Overdrive5_Temperature_Get(m_hADLContext, m_usingAdapterIndex, 0, &adlTemp) != ADL_OK) 
        {
            return false;
        }

        sensorsNum = 1;
        temp[0] = (unsigned int)adlTemp.iTemperature/1000;

        return true;
    }

private:
    ADL2_MAIN_CONTROL_CREATE ADL2_Main_Control_Create; // ADL初始化函数指针
    ADL2_MAIN_CONTROL_DESTROY ADL2_Main_Control_Destroy; // ADL卸载函数指针
    ADL2_ADAPTER_NUMBEROFADAPTERS_GET ADL2_Adapter_NumberOfAdapters_Get; // ADL获取适配器数量函数指针
    ADL2_ADAPTER_ADAPTERINFO_GET ADL2_Adapter_AdapterInfo_Get; // ADL获取适配器信息函数指针
    ADL2_OVERDRIVE5_TEMPERATURE_GET ADL2_Overdrive5_Temperature_Get; // ADL获取显卡温度函数指针

    ADL_CONTEXT_HANDLE m_hADLContext; // ADL环境句柄
    const int INVALID_ADAPTER_INDEX; // 无效显卡适配器索引
    int  m_usingAdapterIndex; // 正在使用的显卡适配器索引
    HMODULE m_hADLDll; // ADL动态库句柄

};

LGpuTemp::LGpuTemp()
{
    m_pGpuTemp = new LNvGpuTemp();
    unsigned int sensorsNum = 0;
    unsigned int temp[MAX_GPU_SENSORS_NUMBER] = {0};
    if (m_pGpuTemp->GetTemp(sensorsNum, temp))
    {
        return;
    }

    if (m_pGpuTemp != 0)
    {
        delete m_pGpuTemp;
        m_pGpuTemp = 0;
    }

    m_pGpuTemp = new LAMDGpuTemp();
    if (m_pGpuTemp->GetTemp(sensorsNum, temp))
    {
        return;
    }

    if (m_pGpuTemp != 0)
    {
        delete m_pGpuTemp;
        m_pGpuTemp = 0;
    }

}

LGpuTemp::~LGpuTemp()
{
    if (m_pGpuTemp != 0)
    {
        delete m_pGpuTemp;
        m_pGpuTemp = 0;
    }
}

bool LGpuTemp::Get(OUT unsigned int& sensorsNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER])
{
    if (m_pGpuTemp == 0)
        return false;

    return m_pGpuTemp->GetTemp(sensorsNum, temp);
}