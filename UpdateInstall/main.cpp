
#include <cstdio>

#include <string>
using std::string;

#include <Windows.h>

#include "unrar.h"

#pragma comment(lib, "unrar.lib")

void DbgPrint(const char *format, ...)
{
    string debugInfor = "<BLHWScaner><UpdateInstall> ";
    
    va_list args;
    va_start(args, format);
    char strBuffer[1024] = {0};
    vsprintf_s(strBuffer, format, args);
    va_end(args);

    debugInfor += strBuffer;

    OutputDebugString(debugInfor.c_str());
}

/// @brief 关闭主进程
void KillBLHWScaner()
{
    // 找到主窗口
    HWND hWnd = FindWindow(NULL, "BLHWScaner");
    if (NULL == hWnd)
        return;

    // 获取进程ID
    DWORD dwProcessID;
    GetWindowThreadProcessId(hWnd, &dwProcessID);

    // 打开进程句柄
    HANDLE hHandle = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);

    // 关闭进程
    if (FALSE != TerminateProcess(hHandle, 0))
    {
        WaitForSingleObject(hHandle, INFINITE);
    }

    CloseHandle(hHandle);
    hHandle = NULL;
}

/// @brief 解压压缩文件
/// @return 成功返回true, 失败返回false
bool DecompressionRarFile()
{
    bool bRet = true;

    RAROpenArchiveData archiveData = {0};
    archiveData.ArcName = "..\\Temp\\Update\\BLHWScaner.rar";
    archiveData.OpenMode = RAR_OM_EXTRACT;

    HANDLE hRarFile = RAROpenArchive(&archiveData);
    if (0 == hRarFile)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    while (true)
    {
        int iRet = 0;
        // 压缩文件头数据
        RARHeaderData headerData = {0};
        iRet = RARReadHeader(hRarFile, &headerData);
        if (0 != iRet)
        {
            bRet = false;
            break;
        }

        // 将文件解压到上级目录中
        string fileName = "..\\";

        // 如果压缩文件包含自身程序, 则修改文件名称: 文件名最后加上N
        if ((0 == strcmp(headerData.FileName, "Update\\UpdateInstall.exe")) ||
            (0 == strcmp(headerData.FileName, "Update\\unrar.dll")))
        {
            fileName +=headerData.FileName;
            fileName += "N";
        }
        else
        {
            fileName += headerData.FileName;
        }


        char fileNameBuffer[260] = {0};
        strcpy_s(fileNameBuffer, fileName.c_str());
        RARProcessFile(hRarFile, RAR_EXTRACT, NULL, fileNameBuffer);
    }



SAFE_EXIT:
    if (hRarFile != NULL)
    {
        RARCloseArchive(hRarFile);
        hRarFile = 0;

        remove("..\\Temp\\Update\\BLHWScaner.rar");
    }

    return bRet;
}

/// @brief 启动主程序
void StartBLHWScaner()
{
    STARTUPINFO si; //一些必备参数设置  
    memset(&si, 0, sizeof(STARTUPINFO));  
    si.cb = sizeof(STARTUPINFO);  
    si.dwFlags = STARTF_USESHOWWINDOW;  
    si.wShowWindow = SW_SHOW;  

    PROCESS_INFORMATION pi; //必备参数设置结束

    CreateProcess(NULL, "..\\BLHWScaner.exe", NULL, NULL, FALSE, 0, NULL, "..\\", &si, &pi);
}


int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
    DbgPrint("Installing new version");

    DbgPrint("Kill main process");
    KillBLHWScaner();

    Sleep(200);

    DbgPrint("Decompress rar file");
    DecompressionRarFile();

    Sleep(200);

    DbgPrint("Start main process");
    StartBLHWScaner();

    return 0;
}