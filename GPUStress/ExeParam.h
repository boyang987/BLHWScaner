#pragma once

#include <string>
using std::string;
using std::wstring;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// <SUMMARY>
/// 执行参数结构
/// </SUMMARY>
struct ExeParam
{
    unsigned int RunTime; // 运行时间, 单位分钟
    wstring DeviceIdx; // 设备索引
    wstring DevicePath;  // 设备路径
};

/// <SUMMARY>
/// 生成默认参数文件
/// </SUMMARY>
void GenerateDefaultParamFile();

/// <SUMMARY>
/// 判断参数文件是否存在
/// </SUMMARY>
bool IsParamFileExist();

/// <SUMMARY>
/// 获取执行参数
/// </SUMMARY>
void GetExeParam(OUT ExeParam& exeParam);