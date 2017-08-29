/*
 * Author: Burnell Liu  Date:  Email: burnell_liu@outlook.com 
 * Description:
 * 该头文件中声明了使用并行加速器计算曼德勃罗特集的方法
 */

#pragma once

#include <string>
using std::wstring;
#include <vector>
using std::vector;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

#ifndef LTEMPLATE
#define LTEMPLATE template<typename Type>
#endif

/// <SUMMARY>
/// 曼德勃罗特图像结构
/// </SUMMARY>
struct MandelbrotImage
{
    unsigned int Width; // 图像宽
    unsigned int Height; // 图像高
    unsigned int* PData; // 存储图像数据, 请自行分配和释放该内存缓冲区, 应该分配Width*Height大小的内存
};

/// <SUMMARY>
/// 曼德勃罗特参数结构
/// </SUMMARY>
LTEMPLATE struct MandelbrotParam
{
    Type RealMin; // 实部最小值
    Type ImgMin; // 虚部最小值
    Type RealMax; // 实部最大值
    Type ImgMax; // 虚部最大值
    unsigned int MaxIter; // 最大迭代次数
};

/// <SUMMARY>
/// 加速器生成曼德勃罗特集(单精度版本)
/// </SUMMARY>
/// <PARAM name="param" dir="IN">
/// 曼德勃罗特计算参数
/// </PARAM>
/// <PARAM name="image" dir="INOUT">
/// 曼德勃罗特图像
/// 计算结束后会填充PData
/// </PARAM>
/// <RETURN>
/// 参数有误返回false
/// </RETURN>
bool AccGenerateMandelbrot(IN const MandelbrotParam<float>& param, INOUT MandelbrotImage& image);

/// <SUMMARY>
/// 加速器生成曼德勃罗特集(双精度版本)
/// </SUMMARY>
/// <PARAM name="param" dir="IN">
/// 曼德勃罗特计算参数
/// </PARAM>
/// <PARAM name="image" dir="INOUT">
/// 曼德勃罗特图像
/// 计算结束后会填充PData
/// </PARAM>
/// <RETURN>
/// 参数有误返回false
/// </RETURN>
bool AccGenerateMandelbrot(IN const MandelbrotParam<double>& param, INOUT MandelbrotImage& image);


/// <SUMMARY>
/// 加速器设备结构
/// </SUMMARY>
struct AccDevice
{
    bool SupportDouble; // 标志是否支持双精度浮点数
    bool IsEmulated; // 标志是否是仿真加速器
    wstring DevicePath; // 设备路径
    wstring DeviceDesc; // 设备描述
};


/// <SUMMARY>
/// 获取所有的加速器设备
/// </SUMMARY>
/// <PARAM name="accDevice" dir="OUT">
/// 存储设备属性
/// </PARAM>
void GetAccelerators(OUT vector<AccDevice>& accDevicesVec);


/// <SUMMARY>
/// 设置指定的设备作为默认加速器
/// </SUMMARY>
/// <PARAM name="devicePath" dir="IN">
/// 指定的设备路径
/// </PARAM>
/// <RETURN>
/// 成功返回true, 失败返回false
/// </RETURN>
bool SetDefaultAccelerator(IN const wstring& devicePath);

/// <SUMMARY>
/// 获取默认加速器
/// </SUMMARY>
/// <PARAM name="accDevice" dir="OUT">
/// 存储加速器属性
/// </PARAM>
void GetDefaultAccelerator(OUT AccDevice& accDevice);
    
