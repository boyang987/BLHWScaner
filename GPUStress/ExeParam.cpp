
#include "ExeParam.h"

#include <Windows.h>

#include "Mandelbrot.h"

#define EXEPARAM_INI_FILE L".\\ExeParam.ini"

#define EXEPARAM_SETION L"ExeParam"
#define TIME_KEY L"Time"
#define DEVICE_KEY L"Device"
#define DEVICE_IDX_DEFAULT L"-1"

#define DEVICE_SETTION L"Device"
#define COUNT_KEY L"Count"

#define DEVICE_PATH_KEY L"DevicePath"


/// <SUMMARY>
/// 字符串转换为宽字符串
/// </SUMMARY>
/// <PARAM name = " strSrc" dir = "IN">
/// 源字符串
/// </PARAM>
/// <PARAM name = " strDest" dir = "OUT">
/// 存储转换后的宽字符串
/// </PARAM>
/// <RETURNS>
/// 成功返回0, 失败返回false
/// </RETURNS>
static bool StringToWString(const string& strSrc, wstring& strDest)
{
    int nSize = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.length(), 0, 0);
    if (nSize <= 0)
        return false;
    wchar_t* pwszDst = new wchar_t[nSize + 1];

    int iRet = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.length(), pwszDst, nSize);

    pwszDst[nSize] = 0;

    strDest.clear();
    strDest.assign(pwszDst);

    delete[] pwszDst;

    return true;
}

void GenerateDefaultParamFile()
{
    DeleteFileW(EXEPARAM_INI_FILE);

    // 配置文件中写入运行时间, 默认为10分钟
    WritePrivateProfileStringW(EXEPARAM_SETION, TIME_KEY, L"10", EXEPARAM_INI_FILE);

    WritePrivateProfileStringW(EXEPARAM_SETION, DEVICE_KEY, DEVICE_IDX_DEFAULT, EXEPARAM_INI_FILE);

    vector<AccDevice> accDevicesVec;
    GetAccelerators(accDevicesVec);

    wchar_t strBuffer[256] = { 0 };
    swprintf_s(strBuffer, L"%u", accDevicesVec.size());
    WritePrivateProfileStringW(DEVICE_SETTION, COUNT_KEY, strBuffer, EXEPARAM_INI_FILE);

    for (unsigned int i = 0; i < accDevicesVec.size(); i++)
    {
        swprintf_s(strBuffer, L"%u", i);
        wstring appName = strBuffer;
        WritePrivateProfileStringW(appName.c_str(), L"DeviceDesc", accDevicesVec[i].DeviceDesc.c_str(), EXEPARAM_INI_FILE);
        WritePrivateProfileStringW(appName.c_str(), DEVICE_PATH_KEY, accDevicesVec[i].DevicePath.c_str(), EXEPARAM_INI_FILE);
        WritePrivateProfileStringW(appName.c_str(), L"IsEmulated", accDevicesVec[i].IsEmulated ? L"True" : L"False", EXEPARAM_INI_FILE);
        WritePrivateProfileStringW(appName.c_str(), L"SupportDouble", accDevicesVec[i].SupportDouble ? L"True" : L"False", EXEPARAM_INI_FILE);
    }
}

bool IsParamFileExist()
{
    WIN32_FIND_DATAW wfd;
    bool rValue = false;
    HANDLE hFind = FindFirstFileW(EXEPARAM_INI_FILE, &wfd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        rValue = true;
    }
    FindClose(hFind);
    return rValue;
}

void GetExeParam(OUT ExeParam& exeParam)
{
    exeParam.DeviceIdx.clear();
    exeParam.RunTime = 0;
    exeParam.DevicePath.clear();

    exeParam.RunTime = GetPrivateProfileIntW(EXEPARAM_SETION, TIME_KEY, 10, EXEPARAM_INI_FILE);

    wchar_t buffer[256] = { 0 };
    GetPrivateProfileStringW(EXEPARAM_SETION, DEVICE_KEY, DEVICE_IDX_DEFAULT, buffer, 256, EXEPARAM_INI_FILE);
    exeParam.DeviceIdx = buffer;

    GetPrivateProfileStringW(exeParam.DeviceIdx.c_str(), DEVICE_PATH_KEY, L"", buffer, 256, EXEPARAM_INI_FILE);
    exeParam.DevicePath = buffer;
}

/*
else
{
// 命令行为其他时, 尝试在配置文件中查询指定节点,
// 如果找不到, 则退出
wstring cmdLineW;
StringToWString(cmdLine, cmdLineW);
wchar_t devPath[256] = { 0 };
DWORD num = GetPrivateProfileStringW(
cmdLineW.c_str(),
L"DevicePath",
L"",
devPath,
256,
DEVICE_INI_FILE);
if (0 == num)
return 0;

SetDefaultAccelerator(devPath);
}
*/