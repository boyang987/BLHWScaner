
#include "LWinRing0.h"

#include <Windows.h>

#include "LLock.h"

// DLL
typedef DWORD (WINAPI *_GetDllStatus) ();
typedef DWORD (WINAPI *_GetDllVersion) (PBYTE major, PBYTE minor, PBYTE revision, PBYTE release);
typedef DWORD (WINAPI *_GetDriverVersion) (PBYTE major, PBYTE minor, PBYTE revision, PBYTE release);
typedef DWORD (WINAPI *_GetDriverType) ();

typedef BOOL (WINAPI *_InitializeOls) ();
typedef VOID (WINAPI *_DeinitializeOls) ();

// CPU
typedef BOOL (WINAPI *_IsCpuid) ();
typedef BOOL (WINAPI *_IsMsr) ();
typedef BOOL (WINAPI *_IsTsc) ();

typedef BOOL  (WINAPI *_Hlt) ();
typedef BOOL (WINAPI *_Rdmsr) (DWORD index, PDWORD eax, PDWORD edx);
typedef DWORD (WINAPI *_Wrmsr) (DWORD index, DWORD eax, DWORD edx);
typedef DWORD (WINAPI *_Rdpmc) (DWORD index, PDWORD eax, PDWORD edx);
typedef BOOL (WINAPI *_Cpuid) (DWORD index, PDWORD eax, PDWORD ebx, PDWORD ecx, PDWORD edx);
typedef DWORD (WINAPI *_Rdtsc) (PDWORD eax, PDWORD edx);

typedef BOOL  (WINAPI *_HltTx) (DWORD_PTR threadAffinityMask);
typedef DWORD (WINAPI *_RdmsrTx) (DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);
typedef DWORD (WINAPI *_WrmsrTx) (DWORD index, DWORD eax, DWORD edx, DWORD_PTR threadAffinityMask);
typedef DWORD (WINAPI *_RdpmcTx) (DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);
typedef DWORD (WINAPI *_CpuidTx) (DWORD index, PDWORD eax, PDWORD ebx, PDWORD ecx, PDWORD edx, DWORD_PTR threadAffinityMask);
typedef DWORD (WINAPI *_RdtscTx) (PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);

typedef BOOL  (WINAPI *_HltPx)   (DWORD_PTR processAffinityMask);
typedef DWORD (WINAPI *_RdmsrPx) (DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR processAffinityMask);
typedef DWORD (WINAPI *_WrmsrPx) (DWORD index, DWORD eax, DWORD edx, DWORD_PTR processAffinityMask);
typedef DWORD (WINAPI *_RdpmcPx) (DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR processAffinityMask);
typedef DWORD (WINAPI *_CpuidPx) (DWORD index, PDWORD eax, PDWORD ebx, PDWORD ecx, PDWORD edx, DWORD_PTR processAffinityMask);
typedef DWORD (WINAPI *_RdtscPx) (PDWORD eax, PDWORD edx, DWORD_PTR processAffinityMask);

// I/O
typedef BYTE  (WINAPI *_ReadIoPortByte) (WORD address);
typedef WORD  (WINAPI *_ReadIoPortWord) (WORD address);
typedef DWORD (WINAPI *_ReadIoPortDword) (WORD address);

typedef BOOL (WINAPI *_ReadIoPortByteEx) (WORD address, PBYTE value);
typedef BOOL (WINAPI *_ReadIoPortWordEx) (WORD address, PWORD value);
typedef BOOL (WINAPI *_ReadIoPortDwordEx) (WORD address, PDWORD value);

typedef VOID (WINAPI *_WriteIoPortByte) (WORD address, BYTE value);
typedef VOID (WINAPI *_WriteIoPortWord) (WORD address, WORD value);
typedef VOID (WINAPI *_WriteIoPortDword) (WORD address, DWORD value);

typedef BOOL (WINAPI *_WriteIoPortByteEx) (WORD address, BYTE value);
typedef BOOL (WINAPI *_WriteIoPortWordEx) (WORD address, WORD value);
typedef BOOL (WINAPI *_WriteIoPortDwordEx) (WORD address, DWORD value);

// PCI
typedef VOID (WINAPI *_SetPciMaxBusIndex) (BYTE max);

typedef BYTE  (WINAPI *_ReadPciConfigByte) (DWORD pciAddress, BYTE regAddress);
typedef WORD  (WINAPI *_ReadPciConfigWord) (DWORD pciAddress, BYTE regAddress);
typedef DWORD (WINAPI *_ReadPciConfigDword) (DWORD pciAddress, BYTE regAddress);

typedef BOOL (WINAPI *_ReadPciConfigByteEx) (DWORD pciAddress, DWORD regAddress, PBYTE value);
typedef BOOL (WINAPI *_ReadPciConfigWordEx) (DWORD pciAddress, DWORD regAddress, PWORD value);
typedef BOOL (WINAPI *_ReadPciConfigDwordEx) (DWORD pciAddress, DWORD regAddress, PDWORD value);

typedef VOID (WINAPI *_WritePciConfigByte) (DWORD pciAddress, BYTE regAddress, BYTE value);
typedef VOID (WINAPI *_WritePciConfigWord) (DWORD pciAddress, BYTE regAddress, WORD value);
typedef VOID (WINAPI *_WritePciConfigDword) (DWORD pciAddress, BYTE regAddress, DWORD value);

typedef BOOL (WINAPI *_WritePciConfigByteEx) (DWORD pciAddress, DWORD regAddress, BYTE value);
typedef BOOL (WINAPI *_WritePciConfigWordEx) (DWORD pciAddress, DWORD regAddress, WORD value);
typedef BOOL (WINAPI *_WritePciConfigDwordEx) (DWORD pciAddress, DWORD regAddress, DWORD value);

typedef DWORD (WINAPI *_FindPciDeviceById) (WORD vendorId, WORD deviceId, BYTE index);
typedef DWORD (WINAPI *_FindPciDeviceByClass) (BYTE baseClass, BYTE subClass, BYTE programIf, BYTE index);

namespace LWinRing0
{
    static HMODULE ghModule = NULL; ///< 动态库模块句柄
    static int gReferenceCount = 0; ///< 记录动态库被引用的次数
    static LManualLock gManualLock; ///< 手动锁, 用于同步

    // DLL
    _GetDllStatus GetDllStatus = NULL;
    _GetDllVersion GetDllVersion = NULL;
    _GetDriverVersion GetDriverVersion = NULL;
    _GetDriverType GetDriverType = NULL;

    _InitializeOls InitializeOls = NULL;
    _DeinitializeOls DeinitializeOls = NULL;

    // CPU
    _IsCpuid IsCpuid = NULL;
    _IsMsr IsMsr = NULL;
    _IsTsc IsTsc = NULL;

    _Hlt Hlt = NULL;
    static _Rdmsr RdmsrFun = NULL;
    _Wrmsr Wrmsr = NULL;
    _Rdpmc Rdpmc = NULL;
    static _Cpuid CpuidFun = NULL;
    _Rdtsc Rdtsc = NULL;

    _HltTx HltTx = NULL;
    _RdmsrTx RdmsrTx = NULL;
    _WrmsrTx WrmsrTx = NULL;
    _RdpmcTx RdpmcTx = NULL;
    _CpuidTx CpuidTx = NULL;
    _RdtscTx RdtscTx = NULL;

    _HltPx HltPx = NULL;
    _RdmsrPx RdmsrPx = NULL;
    _WrmsrPx WrmsrPx = NULL;
    _RdpmcPx RdpmcPx = NULL;
    _CpuidPx CpuidPx = NULL;
    _RdtscPx RdtscPx = NULL;

    // I/O
    _ReadIoPortByte ReadIoPortByte = NULL;
    _ReadIoPortWord ReadIoPortWord = NULL;
    static _ReadIoPortDword ReadIoPortDwordFunc = NULL;

    _ReadIoPortByteEx ReadIoPortByteEx = NULL;
    _ReadIoPortWordEx ReadIoPortWordEx = NULL;
    _ReadIoPortDwordEx ReadIoPortDwordEx = NULL;

    _WriteIoPortByte WriteIoPortByte = NULL;
    _WriteIoPortWord WriteIoPortWord = NULL;
    static _WriteIoPortDword WriteIoPortDwordFunc = NULL;

    _WriteIoPortByteEx WriteIoPortByteEx = NULL;
    _WriteIoPortWordEx WriteIoPortWordEx = NULL;
    _WriteIoPortDwordEx WriteIoPortDwordEx = NULL;

    // PCI
    _SetPciMaxBusIndex SetPciMaxBusIndex = NULL;

    _ReadPciConfigByte ReadPciConfigByte = NULL;
    _ReadPciConfigWord ReadPciConfigWord = NULL;
    _ReadPciConfigDword ReadPciConfigDword = NULL;

    _ReadPciConfigByteEx ReadPciConfigByteEx = NULL;
    _ReadPciConfigWordEx ReadPciConfigWordEx = NULL;
    _ReadPciConfigDwordEx ReadPciConfigDwordEx = NULL;

    _WritePciConfigByte WritePciConfigByte = NULL;
    _WritePciConfigWord WritePciConfigWord = NULL;
    _WritePciConfigDword WritePciConfigDword = NULL;

    _WritePciConfigByteEx WritePciConfigByteEx = NULL;
    _WritePciConfigWordEx WritePciConfigWordEx = NULL;
    _WritePciConfigDwordEx WritePciConfigDwordEx = NULL;

    _FindPciDeviceById FindPciDeviceById = NULL;
    _FindPciDeviceByClass FindPciDeviceByClass = NULL;

    void InitWinRing0Func()
    {
        // DLL
        GetDllStatus =         NULL;
        GetDllVersion =        NULL;
        GetDriverVersion =     NULL;
        GetDriverType =        NULL;

        InitializeOls =        NULL;
        DeinitializeOls =      NULL;

        // CPU             
        IsCpuid =              NULL;
        IsMsr =                NULL;
        IsTsc =                NULL;
        Hlt =                  NULL;
        RdmsrFun =             NULL;
        Wrmsr =                NULL;
        Rdpmc =                NULL;
        CpuidFun =             NULL;
        Rdtsc =                NULL;
        HltTx =                NULL;
        RdmsrTx =              NULL;
        WrmsrTx =              NULL;
        RdpmcTx =              NULL;
        CpuidTx =              NULL;
        RdtscTx =              NULL;
        HltPx =                NULL;
        RdmsrPx =              NULL;
        WrmsrPx =              NULL;
        RdpmcPx =              NULL;
        CpuidPx =              NULL;
        RdtscPx =              NULL;

        // I/O                      
        ReadIoPortByte =       NULL;
        ReadIoPortWord =       NULL;
        ReadIoPortDwordFunc =  NULL;
        ReadIoPortByteEx =     NULL;
        ReadIoPortWordEx =     NULL;
        ReadIoPortDwordEx =    NULL;
        WriteIoPortByte =      NULL;
        WriteIoPortWord =      NULL;
        WriteIoPortDwordFunc = NULL;
        WriteIoPortByteEx =    NULL;
        WriteIoPortWordEx =    NULL;
        WriteIoPortDwordEx =   NULL;

        // PCI                    
        SetPciMaxBusIndex =    NULL;
        ReadPciConfigByte =    NULL;
        ReadPciConfigWord =    NULL;
        ReadPciConfigDword =   NULL;
        ReadPciConfigByteEx =  NULL;
        ReadPciConfigWordEx =  NULL;
        ReadPciConfigDwordEx = NULL;
        WritePciConfigByte =   NULL;
        WritePciConfigWord =   NULL;
        WritePciConfigDword =  NULL;
        WritePciConfigByteEx = NULL;
        WritePciConfigWordEx = NULL;
        WritePciConfigDwordEx= NULL;

        FindPciDeviceById =    NULL;
        FindPciDeviceByClass = NULL;
    }

    void GetWinRing0Func()
    {
        if (NULL == ghModule)
            return;

        //-----------------------------------------------------------------------------
        // GetProcAddress
        //-----------------------------------------------------------------------------
        // DLL
        GetDllStatus =          (_GetDllStatus)         GetProcAddress (ghModule, "GetDllStatus");
        GetDllVersion =         (_GetDllVersion)        GetProcAddress (ghModule, "GetDllVersion");
        GetDriverVersion =      (_GetDriverVersion)     GetProcAddress (ghModule, "GetDriverVersion");
        GetDriverType =         (_GetDriverType)        GetProcAddress (ghModule, "GetDriverType");

        InitializeOls =         (_InitializeOls)        GetProcAddress (ghModule, "InitializeOls");
        DeinitializeOls =       (_DeinitializeOls)      GetProcAddress (ghModule, "DeinitializeOls");

        // CPU
        IsCpuid =               (_IsCpuid)              GetProcAddress (ghModule, "IsCpuid");
        IsMsr =                 (_IsMsr)                GetProcAddress (ghModule, "IsMsr");
        IsTsc =                 (_IsTsc)                GetProcAddress (ghModule, "IsTsc");
        Hlt =                   (_Hlt)                  GetProcAddress (ghModule, "Hlt");
        RdmsrFun =              (_Rdmsr)                GetProcAddress (ghModule, "Rdmsr");
        Wrmsr =                 (_Wrmsr)                GetProcAddress (ghModule, "Wrmsr");
        Rdpmc =                 (_Rdpmc)                GetProcAddress (ghModule, "Rdpmc");
        CpuidFun =              (_Cpuid)                GetProcAddress (ghModule, "Cpuid");
        Rdtsc =                 (_Rdtsc)                GetProcAddress (ghModule, "Rdtsc");
        HltTx =                 (_HltTx)                GetProcAddress (ghModule, "HltTx");
        RdmsrTx =               (_RdmsrTx)              GetProcAddress (ghModule, "RdmsrTx");
        WrmsrTx =               (_WrmsrTx)              GetProcAddress (ghModule, "WrmsrTx");
        RdpmcTx =               (_RdpmcTx)              GetProcAddress (ghModule, "RdpmcTx");
        CpuidTx =               (_CpuidTx)              GetProcAddress (ghModule, "CpuidTx");
        RdtscTx =               (_RdtscTx)              GetProcAddress (ghModule, "RdtscTx");
        HltPx =                 (_HltPx)                GetProcAddress (ghModule, "HltPx");
        RdmsrPx =               (_RdmsrPx)              GetProcAddress (ghModule, "RdmsrPx");
        WrmsrPx =               (_WrmsrPx)              GetProcAddress (ghModule, "WrmsrPx");
        RdpmcPx =               (_RdpmcPx)              GetProcAddress (ghModule, "RdpmcPx");
        CpuidPx =               (_CpuidPx)              GetProcAddress (ghModule, "CpuidPx");
        RdtscPx =               (_RdtscPx)              GetProcAddress (ghModule, "RdtscPx");

        // I/O
        ReadIoPortByte =        (_ReadIoPortByte)       GetProcAddress (ghModule, "ReadIoPortByte");
        ReadIoPortWord =        (_ReadIoPortWord)       GetProcAddress (ghModule, "ReadIoPortWord");
        ReadIoPortDwordFunc =   (_ReadIoPortDword)      GetProcAddress (ghModule, "ReadIoPortDword");

        ReadIoPortByteEx =      (_ReadIoPortByteEx)     GetProcAddress (ghModule, "ReadIoPortByteEx");
        ReadIoPortWordEx =      (_ReadIoPortWordEx)     GetProcAddress (ghModule, "ReadIoPortWordEx");
        ReadIoPortDwordEx =     (_ReadIoPortDwordEx)    GetProcAddress (ghModule, "ReadIoPortDwordEx");

        WriteIoPortByte =       (_WriteIoPortByte)      GetProcAddress (ghModule, "WriteIoPortByte");
        WriteIoPortWord =       (_WriteIoPortWord)      GetProcAddress (ghModule, "WriteIoPortWord");
        WriteIoPortDwordFunc =  (_WriteIoPortDword)     GetProcAddress (ghModule, "WriteIoPortDword");

        WriteIoPortByteEx =     (_WriteIoPortByteEx)    GetProcAddress (ghModule, "WriteIoPortByteEx");
        WriteIoPortWordEx =     (_WriteIoPortWordEx)    GetProcAddress (ghModule, "WriteIoPortWordEx");
        WriteIoPortDwordEx =    (_WriteIoPortDwordEx)   GetProcAddress (ghModule, "WriteIoPortDwordEx");

        // PCI
        SetPciMaxBusIndex =     (_SetPciMaxBusIndex)    GetProcAddress (ghModule, "SetPciMaxBusIndex");

        ReadPciConfigByte =     (_ReadPciConfigByte)    GetProcAddress (ghModule, "ReadPciConfigByte");
        ReadPciConfigWord =     (_ReadPciConfigWord)    GetProcAddress (ghModule, "ReadPciConfigWord");
        ReadPciConfigDword =    (_ReadPciConfigDword)   GetProcAddress (ghModule, "ReadPciConfigDword");

        ReadPciConfigByteEx =   (_ReadPciConfigByteEx)  GetProcAddress (ghModule, "ReadPciConfigByteEx");
        ReadPciConfigWordEx =   (_ReadPciConfigWordEx)  GetProcAddress (ghModule, "ReadPciConfigWordEx");
        ReadPciConfigDwordEx =  (_ReadPciConfigDwordEx) GetProcAddress (ghModule, "ReadPciConfigDwordEx");

        WritePciConfigByte =    (_WritePciConfigByte)   GetProcAddress (ghModule, "WritePciConfigByte");
        WritePciConfigWord =    (_WritePciConfigWord)   GetProcAddress (ghModule, "WritePciConfigWord");
        WritePciConfigDword =   (_WritePciConfigDword)  GetProcAddress (ghModule, "WritePciConfigDword");

        WritePciConfigByteEx =  (_WritePciConfigByteEx) GetProcAddress (ghModule, "WritePciConfigByteEx");
        WritePciConfigWordEx =  (_WritePciConfigWordEx) GetProcAddress (ghModule, "WritePciConfigWordEx");
        WritePciConfigDwordEx = (_WritePciConfigDwordEx)GetProcAddress (ghModule, "WritePciConfigDwordEx");

        FindPciDeviceById =     (_FindPciDeviceById)    GetProcAddress (ghModule, "FindPciDeviceById");
        FindPciDeviceByClass =  (_FindPciDeviceByClass) GetProcAddress (ghModule, "FindPciDeviceByClass");
    }

    bool CheckWinRing0Func()
    {
        //-----------------------------------------------------------------------------
        // Check Functions
        //-----------------------------------------------------------------------------
        if(!(
                GetDllStatus
            &&  GetDllVersion
            &&  GetDriverVersion
            &&  GetDriverType
            &&  InitializeOls
            &&  DeinitializeOls
            &&  IsCpuid
            &&  IsMsr
            &&  IsTsc
            &&  Hlt
            &&  HltTx
            &&  HltPx
            &&  RdmsrFun
            &&  RdmsrTx
            &&  RdmsrPx
            &&  Wrmsr
            &&  WrmsrTx
            &&  WrmsrPx
            &&  Rdpmc
            &&  RdpmcTx
            &&  RdpmcPx
            &&  CpuidFun
            &&  CpuidTx
            &&  CpuidPx
            &&  Rdtsc
            &&  RdtscTx
            &&  RdtscPx
            &&  ReadIoPortByte
            &&  ReadIoPortWord
            &&  ReadIoPortDwordFunc
            &&  ReadIoPortByteEx
            &&  ReadIoPortWordEx
            &&  ReadIoPortDwordEx
            &&  WriteIoPortByte
            &&  WriteIoPortWord
            &&  WriteIoPortDwordFunc
            &&  WriteIoPortByteEx
            &&  WriteIoPortWordEx
            &&  WriteIoPortDwordEx
            &&  SetPciMaxBusIndex
            &&  ReadPciConfigByte
            &&  ReadPciConfigWord
            &&  ReadPciConfigDword
            &&  ReadPciConfigByteEx
            &&  ReadPciConfigWordEx
            &&  ReadPciConfigDwordEx
            &&  WritePciConfigByte
            &&  WritePciConfigWord 
            &&  WritePciConfigDword
            &&  WritePciConfigByteEx
            &&  WritePciConfigWordEx 
            &&  WritePciConfigDwordEx
            &&  FindPciDeviceById
            &&  FindPciDeviceByClass
            ))
        {
            return false;
        }

        return true;
    }

    bool InitializeWinRing0(const wstring& filePath)
    {
        // 申请锁对象, 用于同步
        LAutoLock autoLock(&gManualLock);

        bool bRet = false;

        // 已经完成初始化, 则退出
        if (NULL != ghModule)
        {
            bRet = true;
            goto SAFE_EXIT;
        }

        ghModule = LoadLibraryW(filePath.c_str());
        if (ghModule == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        GetWinRing0Func();

        if (CheckWinRing0Func() == false)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        

        if (InitializeOls() == TRUE)
        {
            bRet = true;
        }
        else
        {
            bRet = false;
        }



SAFE_EXIT:
        if (!bRet)
        {
            InitWinRing0Func();

            if (ghModule != NULL)
            {
                FreeLibrary(ghModule);
                ghModule = NULL;
            }
        }
        else
        {
            gReferenceCount++;
        }


        return bRet;
    }

    void DeinitializeWinRing0()
    {
        // 申请锁对象, 用于同步
        LAutoLock autoLock(&gManualLock);

        if (0 == gReferenceCount)
            return;

        gReferenceCount -= 1;

        if (gReferenceCount > 0)
            return;



        if (DeinitializeOls != NULL)
        {
            DeinitializeOls();
        }

        InitWinRing0Func();

        if (ghModule != NULL)
        {
            FreeLibrary(ghModule);
            ghModule = NULL;
        }
    }

    int Cpuid(unsigned long index, unsigned long* eax, unsigned long* ebx, unsigned long* ecx, unsigned long* edx)
    {
        if (CpuidFun == NULL)
            return 0;

        return CpuidFun(index, eax, ebx, ecx, edx);
    }

    int Rdmsr(IN unsigned long index, OUT unsigned long* eax, OUT unsigned long* edx)
    {
        if (RdmsrFun == NULL)
            return 0;

        return RdmsrFun(index, eax, edx);
    }

    unsigned long ReadIoPortDword(IN unsigned short address)
    {
        if (NULL == ReadIoPortDwordFunc)
            return 0;

        return ReadIoPortDwordFunc(address);
    }

    void WriteIoPortDword(IN unsigned short address, IN unsigned long value)
    {
        if (NULL == WriteIoPortDwordFunc)
            return;

        WriteIoPortDwordFunc(address, value);
    }
}
