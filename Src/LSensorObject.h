/// @file LSensorObject.h
/// @brief 传感器对象 
/// 
/// Detail: 该文件中声明的类LSensorObject用于加载SensorObject.dll文件来获取传感器数据
/// @author Burnell Liu Email:burnell_liu@outlook.com
/// @version   
/// @date 2016:3:4

#ifndef _LSENSOROBJECT_H_
#define _LSENSOROBJECT_H_

#include <string>
using std::string;
using std::wstring;

// 输入
#ifndef IN
#define IN 
#endif

// 输出
#ifndef OUT
#define OUT 
#endif

// 最大传感器数目
#define MAX_SENSOR_NUMBER 8

// 最大字符串长度
#define MAX_STRING_LEN 256

extern "C"
{
    /// <SUMMARY>
    /// 重力加速度传感器(3D)数据
    /// </SUMMARY>
    struct SAccelerometer3DSensorData
    {
        double X_G; // X轴的重力加速度(G)
        double Y_G; // Y轴的重力加速度(G)
        double Z_G; // Z轴的重力加速度(G)
    };

    /// <SUMMARY>
    /// 重力加速度传感器(3D)信息组
    /// </SUMMARY>
    struct SAccelerometer3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SAccelerometer3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 陀螺仪传感器(3D)数据
    /// </SUMMARY>
    struct SGyrometer3DSensorData
    {
        double X_DEGREES_PER_SECOND; // X轴角速度(度每秒)
        double Y_DEGREES_PER_SECOND; // Y轴角速度(度每秒)
        double Z_DEGREES_PER_SECOND; // Z轴角速度(度每秒)
    };

    /// <SUMMARY>
    /// 陀螺仪传感器(3D)信息组
    /// </SUMMARY>
    struct SGyrometer3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SGyrometer3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 指南针传感器(3D)数据
    /// </SUMMARY>
    struct SCompass3DSensorData
    {
        double X_MILLIGAUSS; // X轴的磁场强度(毫高斯)
        double Y_MILLIGAUSS; // Y轴的磁场强度(毫高斯)
        double Z_MILLIGAUSS; // Z轴的磁场强度(毫高斯)
    };

    /// <SUMMARY>
    /// 指南针传感器(3D)信息组
    /// </SUMMARY>
    struct SCompass3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SCompass3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 光传感器数据
    /// </SUMMARY>
    struct SAmbientLightSensorData
    {
        float LIGHT_LEVEL; // 光照强度
    };

    /// <SUMMARY>
    /// 光传感器信息组
    /// </SUMMARY>
    struct SAmbientLightInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SAmbientLightSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// GPS传感器数据
    /// </SUMMARY>
    struct SGpsSensorData
    {
        double LAYITUDE_DEGREES; // 纬度值
        double LONGITUDE_GERREES; // 经度值
    };

    /// <SUMMARY>
    /// GPS传感器信息组
    /// </SUMMARY>
    struct SGpsInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SGpsSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };
};


class CSensorObject;

/// @brief 传感器对象
class LSensorObject
{
public:
    /// @brief 构造函数
    LSensorObject();

    /// @brief 析构函数
    ~LSensorObject();

    /// <SUMMARY>
    /// 获取重力加速度传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetAccelerometer3DInfor(OUT SAccelerometer3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取陀螺仪传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetGyrometer3DInfor(OUT SGyrometer3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取指南针传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetCompass3DInfor(OUT SCompass3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取光传感器信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetAmbientLightInfor(OUT SAmbientLightInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取GPS传感器信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetGpsInfor(OUT SGpsInforArray* pInforArray);

private:
    CSensorObject* m_pSensorObject; ///< 传感器实现对象
};

#endif