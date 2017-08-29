
#define _EXPORTING_SENSOR
#include "SensorObject.h"

#include <string>
using std::string;
using std::wstring;

#include <Sensorsapi.h>
#include <Sensors.h>

#include <objbase.h>

#include "DebugPrint.h"

/*
注意不能在DLLMain中释放Sensor对象，
不然可能会导致线程卡死
*/


#pragma comment(lib, "Sensorsapi.lib")

#define SAFE_RELEASE(p) do {if (p){ (p)->Release(); (p) = 0;}}while(0)
#define SAFE_DEL_ARRAY(p) do {if (p){ delete[] (p); (p) = 0;}}while(0)

/// <SUMMARY>
/// 初始化COM类(单线程套间)
/// </SUMMARY>
class CComInitS
{
public:
    CComInitS()
        : m_success(false)
    {
        // CoInitialize调用后返回S_FALSE或S_OK后都需调用CoUninitialize
        // CoInitialize调用后返回RPC_E_CHANGE_MODE, 表明当前线程已被初始化(且和当前模式不同),不需要调用CoUninitialize
        HRESULT hr = CoInitialize(0);
        if (hr == S_FALSE || hr == S_OK)
        {
            m_success = true;
        }
    }

    ~CComInitS()
    {
        if (m_success)
            CoUninitialize();
    }
private:
    bool m_success; // 标识COM库是否初始化成功
    CComInitS(const CComInitS&);
    CComInitS& operator = (const CComInitS&);
};

/// <SUMMARY>
/// 传感器基本对象
/// </SUMMARY>
class CSensorObject
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    CSensorObject()
    {
        m_sensorCount = 0;

        m_pSensorManager = NULL;
        m_pSensorCollection = NULL;
        m_pSensorArray = NULL;
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    virtual ~CSensorObject()
    {
        this->SafeRelease();
    }

    /// <SUMMARY>
    /// 初始化Sensor对象
    /// </SUMMARY>
    /// <PARAM name = "sensorType" dir = "IN">
    /// 指定Sensor的GUID
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, m_errorMessage中存储错误信息
    /// </RETURNS>
    bool TypeInit(IN REFSENSOR_TYPE_ID sensorType)
    {
        this->SafeRelease();

        HRESULT hr = CoCreateInstance(CLSID_SensorManager, NULL, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pSensorManager));
        if (hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DISABLED_BY_POLICY))
        {
            this->SafeRelease();
            m_errorMessage = "CoCreateInstance: Unable to retrieve sensor manager due to group policy settings";
            return false;
        }

        // Get the sensor collection
        hr = m_pSensorManager->GetSensorsByType(sensorType, &m_pSensorCollection);
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            this->SafeRelease();
            m_errorMessage = "ISensorManager::GetSensorsByType: No sensors are available for the specified type";
            return false;
        }

        m_pSensorCollection->GetCount(&m_sensorCount);

        if (m_sensorCount == 0)
        {
            this->SafeRelease();
            m_errorMessage = "Sensors count is Zero, no sensors of the requested type";
            return false;
        }

        m_pSensorArray = new ISensor*[m_sensorCount];
        for (ULONG i = 0; i < m_sensorCount; i++)
            m_pSensorArray[i] = NULL;

        for (ULONG i = 0; i < m_sensorCount; i++)
        {
            m_pSensorCollection->GetAt(i, &m_pSensorArray[i]);
        }

        SensorState sensorState = SENSOR_STATE_NOT_AVAILABLE;
        for (ULONG i = 0; i < m_sensorCount; i++)
        {
            m_pSensorArray[i]->GetState(&sensorState);
            if (sensorState == SENSOR_STATE_ACCESS_DENIED)
            {
                hr = m_pSensorManager->RequestPermissions(0, m_pSensorCollection, TRUE);

                if (hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)
                    || hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
                {
                    this->SafeRelease();
                    m_errorMessage = "ISensorManager::RequestPermissions: User have previously denied access to this sensor";
                    return false;
                }
            }
            sensorState = SENSOR_STATE_NOT_AVAILABLE;
        }

        return true;
    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetSensorCount()
    {
        return m_sensorCount;
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        name.clear();

        if (index >= m_sensorCount || index < 0)
        {
            m_errorMessage = "Index is out of range";
            return false;
        }

        ISensor* pSensor = m_pSensorArray[index];


        BSTR friendlyNameBstr = NULL;
        HRESULT hr = pSensor->GetFriendlyName(&friendlyNameBstr);

        if (friendlyNameBstr != NULL)
        {
            wstring friendlyNameWstr(friendlyNameBstr, SysStringLen(friendlyNameBstr));
            name = friendlyNameWstr;
        }

        SysFreeString(friendlyNameBstr);

        if (hr != S_OK)
        {
            return false;
        }

        return true;
    }

    /// <SUMMARY>
    /// 获取传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "pDataReportAddr" dir = "OUT">
    /// 存储传感器数据报告
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetDataReport(IN unsigned int index, OUT ISensorDataReport*& pDataReport)
    {
        if (index >= m_sensorCount || index < 0)
        {
            m_errorMessage = "Index is out of range";
            return false;
        }

        ISensor* pSensor = m_pSensorArray[index];


        HRESULT hr = pSensor->GetData(&pDataReport);
        if (hr == E_INVALIDARG)
        {
            m_errorMessage = "ISensor::GetData: The sensor driver provided badly formed data";
            return false;
        }
        if (hr == HRESULT_FROM_WIN32(ERROR_NO_DATA))
        {
            m_errorMessage = "ISensor::GetData: The sensor has no data to report";
            return false;
        }

        return true;
    }

private:
    /// <SUMMARY>
    /// 释放资源
    /// </SUMMARY>
    void SafeRelease()
    {
        DebugPrint("Enter CSensorObject::SafeRelease\n");
        ISensor* pSensor = NULL;
        for (unsigned long i = 0; i < m_sensorCount; i++)
        {
            pSensor = m_pSensorArray[i];
            SAFE_RELEASE(pSensor);
            DebugPrint("Delete Sensor\n");
            m_pSensorArray[i] = NULL;
        }
        SAFE_DEL_ARRAY(m_pSensorArray);

        m_sensorCount = 0;

        SAFE_RELEASE(m_pSensorCollection);
        DebugPrint("Delete SensorCollection\n");
        SAFE_RELEASE(m_pSensorManager);
        DebugPrint("Delete SensorManager\n");

        DebugPrint("Leave CSensorObject::SafeRelease\n");
    }

private:
    unsigned long m_sensorCount; // Sensor设备数量
    string m_errorMessage; // 错误信息
    ISensor** m_pSensorArray; // 保存Sensor设备对象指针
    CComInitS comInittS; // 用于初始化COM
    ISensorManager* m_pSensorManager;
    ISensorCollection* m_pSensorCollection;

private:
    CSensorObject(const CSensorObject&);
    CSensorObject& operator = (const CSensorObject&);
};

/// <SUMMARY>
/// 重力加速度传感器
/// </SUMMARY>
class LAccelerometer3DSensor
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    LAccelerometer3DSensor()
    {
        m_sensorObject.TypeInit(SENSOR_TYPE_ACCELEROMETER_3D);
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    ~LAccelerometer3DSensor()
    {

    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetCount()
    {
        return m_sensorObject.GetSensorCount();
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        return m_sensorObject.GetFriendlyName(index, name);
    }

    /// <SUMMARY>
    /// 获取重力加速度传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "data" dir = "OUT">
    /// 存储传感器数据
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetData(IN unsigned int index, OUT SAccelerometer3DSensorData& data)
    {
        ISensorDataReport* pSensorDataReport = NULL;
        bool bRet = m_sensorObject.GetDataReport(index, pSensorDataReport);
        if (!bRet)
            return false;

        if (pSensorDataReport == NULL)
            return false;

        PROPVARIANT varX = {};
        PROPVARIANT varY = {};
        PROPVARIANT varZ = {};

        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_X_G, &varX);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Y_G, &varY);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Z_G, &varZ);

        data.X_G = varX.dblVal;
        data.Y_G = varY.dblVal;
        data.Z_G = varZ.dblVal;

        PropVariantClear(&varX);
        PropVariantClear(&varY);
        PropVariantClear(&varZ);

        SAFE_RELEASE(pSensorDataReport);

        return true;

    }

private:
    CSensorObject m_sensorObject; // 传感器基本对象

};

/// <SUMMARY>
/// 陀螺仪传感器
/// </SUMMARY>
class LGyrometer3DSensor
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    LGyrometer3DSensor()
    {
        m_sensorObject.TypeInit(SENSOR_TYPE_GYROMETER_3D);
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    ~LGyrometer3DSensor()
    {

    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetCount()
    {
        return m_sensorObject.GetSensorCount();
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        return m_sensorObject.GetFriendlyName(index, name);
    }

    /// <SUMMARY>
    /// 获取陀螺仪传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "data" dir = "OUT">
    /// 存储传感器数据
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetData(IN unsigned int index, OUT SGyrometer3DSensorData& data)
    {
        ISensorDataReport* pSensorDataReport = NULL;
        bool bRet = m_sensorObject.GetDataReport(index, pSensorDataReport);
        if (!bRet)
            return false;

        if (pSensorDataReport == NULL)
            return false;

        PROPVARIANT varX = {};
        PROPVARIANT varY = {};
        PROPVARIANT varZ = {};

        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ANGULAR_VELOCITY_X_DEGREES_PER_SECOND, &varX);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Y_DEGREES_PER_SECOND, &varY);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Z_DEGREES_PER_SECOND, &varZ);

        data.X_DEGREES_PER_SECOND = varX.dblVal;
        data.Y_DEGREES_PER_SECOND = varY.dblVal;
        data.Z_DEGREES_PER_SECOND = varZ.dblVal;

        PropVariantClear(&varX);
        PropVariantClear(&varY);
        PropVariantClear(&varZ);

        SAFE_RELEASE(pSensorDataReport);

        return true;
    }

private:
    CSensorObject m_sensorObject; // 传感器基本对象
};


/// <SUMMARY>
/// 指南针传感器
/// </SUMMARY>
class LCompass3DSensor
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    LCompass3DSensor()
    {
        m_sensorObject.TypeInit(SENSOR_TYPE_COMPASS_3D);
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    ~LCompass3DSensor()
    {

    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetCount()
    {
        return m_sensorObject.GetSensorCount();
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        return m_sensorObject.GetFriendlyName(index, name);
    }

    /// <SUMMARY>
    /// 获取指南针传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "data" dir = "OUT">
    /// 存储传感器数据
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetData(IN unsigned int index, OUT SCompass3DSensorData& data)
    {
        ISensorDataReport* pSensorDataReport = NULL;
        bool bRet = m_sensorObject.GetDataReport(index, pSensorDataReport);
        if (!bRet)
            return false;

        if (pSensorDataReport == NULL)
            return false;

        PROPVARIANT varX = {};
        PROPVARIANT varY = {};
        PROPVARIANT varZ = {};

        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_MAGNETIC_FIELD_STRENGTH_X_MILLIGAUSS, &varX);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_MAGNETIC_FIELD_STRENGTH_Y_MILLIGAUSS, &varY);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_MAGNETIC_FIELD_STRENGTH_Z_MILLIGAUSS, &varZ);

        data.X_MILLIGAUSS = varX.dblVal;
        data.Y_MILLIGAUSS = varY.dblVal;
        data.Z_MILLIGAUSS = varZ.dblVal;

        PropVariantClear(&varX);
        PropVariantClear(&varY);
        PropVariantClear(&varZ);

        SAFE_RELEASE(pSensorDataReport);

        return true;
    }

private:
    CSensorObject m_sensorObject; // 传感器基本对象
};

/// <SUMMARY>
/// GPS传感器
/// </SUMMARY>
class LLocationGpsSensor
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    LLocationGpsSensor()
    {
        m_sensorObject.TypeInit(SENSOR_TYPE_LOCATION_GPS);
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    ~LLocationGpsSensor()
    {

    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetCount()
    {
        return m_sensorObject.GetSensorCount();
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        return m_sensorObject.GetFriendlyName(index, name);
    }

    /// <SUMMARY>
    /// 获取GPS传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "data" dir = "OUT">
    /// 存储传感器数据
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetData(IN unsigned int index, OUT SGpsSensorData& data)
    {
        ISensorDataReport* pSensorDataReport = NULL;
        bool bRet = m_sensorObject.GetDataReport(index, pSensorDataReport);
        if (!bRet)
            return false;

        if (pSensorDataReport == NULL)
            return false;

        PROPVARIANT varLatitude = {};
        PROPVARIANT varLongitude = {};

        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_LATITUDE_DEGREES, &varLatitude);
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_LONGITUDE_DEGREES, &varLongitude);

        data.LAYITUDE_DEGREES = varLatitude.dblVal;
        data.LONGITUDE_GERREES = varLongitude.dblVal;

        PropVariantClear(&varLatitude);
        PropVariantClear(&varLongitude);

        SAFE_RELEASE(pSensorDataReport);

        return true;
    }

private:
    CSensorObject m_sensorObject; // 基本传感器对象
};


/// <SUMMARY>
/// 光传感器
/// </SUMMARY>
class LAmbientLightSensor
{
public:
    /// <SUMMARY>
    /// 构造函数
    /// </SUMMARY>
    LAmbientLightSensor()
    {
        m_sensorObject.TypeInit(SENSOR_TYPE_AMBIENT_LIGHT);
    }

    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    ~LAmbientLightSensor()
    {

    }

    /// <SUMMARY>
    /// 获取传感器数量
    /// </SUMMARY>
    /// <RETURNS>
    /// 传感器数量
    /// </RETURNS>
    unsigned int GetCount()
    {
        return m_sensorObject.GetSensorCount();
    }

    /// <SUMMARY>
    /// 获取传感器友好名称
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储传感器友好名称
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetFriendlyName(IN unsigned int index, OUT wstring& name)
    {
        return m_sensorObject.GetFriendlyName(index, name);
    }

    /// <SUMMARY>
    /// 获取光传感器数据
    /// </SUMMARY>
    /// <PARAM name = "index" dir = "IN">
    /// 传感器设备索引
    /// </PARAM>
    /// <PARAM name = "data" dir = "OUT">
    /// 存储传感器数据
    /// </PARAM>
    /// <RETURNS>
    /// true(获取成功), false(获取失败)
    /// </RETURNS>
    bool GetData(IN unsigned int index, OUT SAmbientLightSensorData& data)
    {
        ISensorDataReport* pSensorDataReport = NULL;
        bool bRet = m_sensorObject.GetDataReport(index, pSensorDataReport);
        if (!bRet)
            return false;

        if (pSensorDataReport == NULL)
            return false;

        PROPVARIANT varLighLevel = {};
        pSensorDataReport->GetSensorValue(SENSOR_DATA_TYPE_LIGHT_LEVEL_LUX, &varLighLevel);

        data.LIGHT_LEVEL = varLighLevel.fltVal;

        PropVariantClear(&varLighLevel);
        SAFE_RELEASE(pSensorDataReport);

        return true;
    }

private:
    CSensorObject m_sensorObject; // 基本传感器对象

};

bool GetAccelerometer3DSensorInfor(OUT SAccelerometer3DInforArray* pInforArray)
{
    LAccelerometer3DSensor* pAccelerometer3D = NULL;

    if (pInforArray == NULL)
        return false;

    if (pAccelerometer3D == NULL)
    {
        pAccelerometer3D = new LAccelerometer3DSensor();
    }

    ZeroMemory(pInforArray->FriendlyName, MAX_SENSOR_NUMBER * MAX_STRING_LEN);

    pInforArray->Count = pAccelerometer3D->GetCount();
    for (unsigned int i = 0; i < pInforArray->Count && i < MAX_SENSOR_NUMBER; i++)
    {
        pAccelerometer3D->GetData(i, pInforArray->Data[i]);
        wstring name;
        pAccelerometer3D->GetFriendlyName(i, name);
        wcscpy_s(pInforArray->FriendlyName[i], MAX_STRING_LEN, name.c_str());
    }

    if (pAccelerometer3D != NULL)
    {
        delete pAccelerometer3D;
        pAccelerometer3D = NULL;
        DebugPrint("Delete Accelerometer3D Object\n");
    }

    return true;
}

bool GetGyrometer3DSensorInfor(OUT SGyrometer3DInforArray* pInforArray)
{
    LGyrometer3DSensor* pGyrometer3D = NULL;

    if (pInforArray == NULL)
        return false;

    if (pGyrometer3D == NULL)
    {
        pGyrometer3D = new LGyrometer3DSensor();
    }

    ZeroMemory(pInforArray->FriendlyName, MAX_SENSOR_NUMBER * MAX_STRING_LEN);

    pInforArray->Count = pGyrometer3D->GetCount();
    for (unsigned int i = 0; i < pInforArray->Count && i < MAX_SENSOR_NUMBER; i++)
    {
        pGyrometer3D->GetData(i, pInforArray->Data[i]);

        wstring name;
        pGyrometer3D->GetFriendlyName(i, name);
        wcscpy_s(pInforArray->FriendlyName[i], MAX_STRING_LEN, name.c_str());
    }

    if (pGyrometer3D != NULL)
    {
        delete pGyrometer3D;
        pGyrometer3D = NULL;
        DebugPrint("Delete Gyrometer3D Object\n");
    }

    return true;
}

bool GetCompass3DSensorInfor(OUT SCompass3DInforArray* pInforArray)
{
    LCompass3DSensor* pCompass3D = NULL;

    if (pInforArray == NULL)
        return false;

    if (pCompass3D == NULL)
    {
        pCompass3D = new LCompass3DSensor();
    }

    ZeroMemory(pInforArray->FriendlyName, MAX_SENSOR_NUMBER * MAX_STRING_LEN);

    pInforArray->Count = pCompass3D->GetCount();
    for (unsigned int i = 0; i < pInforArray->Count && i < MAX_SENSOR_NUMBER; i++)
    {
        pCompass3D->GetData(i, pInforArray->Data[i]);

        wstring name;
        pCompass3D->GetFriendlyName(i, name);
        wcscpy_s(pInforArray->FriendlyName[i], MAX_STRING_LEN, name.c_str());
    }

    if (pCompass3D != NULL)
    {
        delete pCompass3D;
        pCompass3D = NULL;
        DebugPrint("Delete Compass3D Object\n");
    }

    return true;
}

bool GetGpsSensorInfor(OUT SGpsInforArray* pInforArray)
{
    LLocationGpsSensor* pLocationGps = NULL;

    if (pInforArray == NULL)
        return false;

    if (pLocationGps == NULL)
    {
        pLocationGps = new LLocationGpsSensor();
    }

    ZeroMemory(pInforArray->FriendlyName, MAX_SENSOR_NUMBER * MAX_STRING_LEN);

    pInforArray->Count = pLocationGps->GetCount();
    for (unsigned int i = 0; i < pInforArray->Count && i < MAX_SENSOR_NUMBER; i++)
    {
        pLocationGps->GetData(i, pInforArray->Data[i]);

        wstring name;
        pLocationGps->GetFriendlyName(i, name);
        wcscpy_s(pInforArray->FriendlyName[i], MAX_STRING_LEN, name.c_str());
    }

    if (pLocationGps != NULL)
    {
        delete pLocationGps;
        pLocationGps = NULL;
        DebugPrint("Delete LocationGps Object\n");
    }

    return true;
}

bool GetAmbientLightSensorInfor(OUT SAmbientLightInforArray* pInforArray)
{
    LAmbientLightSensor* pAmbientLight = NULL;

    if (pInforArray == NULL)
        return false;

    if (pAmbientLight == NULL)
    {
        pAmbientLight = new LAmbientLightSensor();
    }

    ZeroMemory(pInforArray->FriendlyName, MAX_SENSOR_NUMBER * MAX_STRING_LEN);

    pInforArray->Count = pAmbientLight->GetCount();
    for (unsigned int i = 0; i < pInforArray->Count && i < MAX_SENSOR_NUMBER; i++)
    {
        pAmbientLight->GetData(i, pInforArray->Data[i]);

        wstring name;
        pAmbientLight->GetFriendlyName(i, name);
        wcscpy_s(pInforArray->FriendlyName[i], MAX_STRING_LEN, name.c_str());
    }

    if (pAmbientLight != NULL)
    {
        delete pAmbientLight;
        pAmbientLight = NULL;
        DebugPrint("Delete AmbientLight Object\n");
    }

    return true;
}



