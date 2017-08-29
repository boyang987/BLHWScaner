


#ifndef _SENSOROBJECT_H_
#define _SENSOROBJECT_H_

#ifdef _EXPORTING_SENSOR
#define SENSOR_API_DECLSPEC __declspec(dllexport)
#else
#define SENSOR_API_DECLSPEC __declspec(dllimport)
#endif

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
    struct SENSOR_API_DECLSPEC SAccelerometer3DSensorData
    {
        double X_G; // X轴的重力加速度(G)
        double Y_G; // Y轴的重力加速度(G)
        double Z_G; // Z轴的重力加速度(G)
    };

    /// <SUMMARY>
    /// 重力加速度传感器(3D)信息组
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SAccelerometer3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SAccelerometer3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 陀螺仪传感器(3D)数据
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SGyrometer3DSensorData
    {
        double X_DEGREES_PER_SECOND; // X轴角速度(度每秒)
        double Y_DEGREES_PER_SECOND; // Y轴角速度(度每秒)
        double Z_DEGREES_PER_SECOND; // Z轴角速度(度每秒)
    };

    /// <SUMMARY>
    /// 陀螺仪传感器(3D)信息组
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SGyrometer3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SGyrometer3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 指南针传感器(3D)数据
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SCompass3DSensorData
    {
        double X_MILLIGAUSS; // X轴的磁场强度(毫高斯)
        double Y_MILLIGAUSS; // Y轴的磁场强度(毫高斯)
        double Z_MILLIGAUSS; // Z轴的磁场强度(毫高斯)
    };

    /// <SUMMARY>
    /// 指南针传感器(3D)信息组
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SCompass3DInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SCompass3DSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 光传感器数据
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SAmbientLightSensorData
    {
        float LIGHT_LEVEL; // 光照强度
    };

    /// <SUMMARY>
    /// 光传感器信息组
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SAmbientLightInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SAmbientLightSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// GPS传感器数据
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SGpsSensorData
    {
        double LAYITUDE_DEGREES; // 纬度值
        double LONGITUDE_GERREES; // 经度值
    };

    /// <SUMMARY>
    /// GPS传感器信息组
    /// </SUMMARY>
    struct SENSOR_API_DECLSPEC SGpsInforArray
    {
        unsigned int Count; // 传感器数量
        wchar_t FriendlyName[MAX_SENSOR_NUMBER][MAX_STRING_LEN]; // 友好名称
        SGpsSensorData Data[MAX_SENSOR_NUMBER]; // 传感器数据
    };

    /// <SUMMARY>
    /// 获取重力加速度传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    SENSOR_API_DECLSPEC bool GetAccelerometer3DSensorInfor(OUT SAccelerometer3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取陀螺仪传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    SENSOR_API_DECLSPEC bool GetGyrometer3DSensorInfor(OUT SGyrometer3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取指南针传感器(3D)信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    SENSOR_API_DECLSPEC bool GetCompass3DSensorInfor(OUT SCompass3DInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取光传感器信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    SENSOR_API_DECLSPEC bool GetAmbientLightSensorInfor(OUT SAmbientLightInforArray* pInforArray);

    /// <SUMMARY>
    /// 获取GPS传感器信息
    /// </SUMMARY>
    /// <PARAM name = "pInforArray" dir = "OUT">
    /// 存储传感器信息
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    SENSOR_API_DECLSPEC bool GetGpsSensorInfor(OUT SGpsInforArray* pInforArray);
}


#endif