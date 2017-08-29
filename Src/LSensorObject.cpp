
#include "LSensorObject.h"

#include <Windows.h>


typedef bool (*GetAccelerometer3DSensorInforFun)(OUT SAccelerometer3DInforArray* pInforArray);

typedef bool (*GetGyrometer3DSensorInforFun)(OUT SGyrometer3DInforArray* pInforArray);

typedef bool (*GetCompass3DSensorInforFun)(OUT SCompass3DInforArray* pInforArray);

typedef bool (*GetAmbientLightSensorInforFun)(OUT SAmbientLightInforArray* pInforArray);

typedef bool (*GetGpsSensorInforFun)(OUT SGpsInforArray* pInforArray);

/// @brief 传感器实现对象
class CSensorObject
{
public:
    CSensorObject()
    {
        m_hSensorObject = NULL;

        m_pGetAccelerometer3DSensorInforFun = NULL;
        m_pGetGyrometer3DSensorInforFun = NULL;
        m_pGetCompass3DSensorInforFun = NULL;
        m_pGetAmbientLightSensorInforFun = NULL;
        m_pGetGpsSensorInforFun = NULL;

        m_hSensorObject = LoadLibraryW(L".\\Sensor\\SensorObject.dll");

        if (m_hSensorObject == NULL)
            return;

        m_pGetAccelerometer3DSensorInforFun = 
            (GetAccelerometer3DSensorInforFun)GetProcAddress(m_hSensorObject, "GetAccelerometer3DSensorInfor");

        m_pGetGyrometer3DSensorInforFun = 
            (GetGyrometer3DSensorInforFun)GetProcAddress(m_hSensorObject, "GetGyrometer3DSensorInfor");

        m_pGetCompass3DSensorInforFun = 
            (GetCompass3DSensorInforFun)GetProcAddress(m_hSensorObject, "GetCompass3DSensorInfor");

        m_pGetAmbientLightSensorInforFun = 
            (GetAmbientLightSensorInforFun)GetProcAddress(m_hSensorObject, "GetAmbientLightSensorInfor");

        m_pGetGpsSensorInforFun = 
            (GetGpsSensorInforFun)GetProcAddress(m_hSensorObject, "GetGpsSensorInfor");
        
    }

    ~CSensorObject()
    {
        if (m_hSensorObject != NULL)
        {
            FreeLibrary(m_hSensorObject);
            m_hSensorObject = NULL;
        }
    }

    bool GetAccelerometer3DInfor(OUT SAccelerometer3DInforArray* pInforArray)
    {
        if (m_pGetAccelerometer3DSensorInforFun == NULL)
            return false;

        return m_pGetAccelerometer3DSensorInforFun(pInforArray);
    }

    bool GetGyrometer3DInfor(OUT SGyrometer3DInforArray* pInforArray)
    {
        if (m_pGetGyrometer3DSensorInforFun == NULL)
            return false;

        return m_pGetGyrometer3DSensorInforFun(pInforArray);
    }

    bool GetCompass3DInfor(OUT SCompass3DInforArray* pInforArray)
    {
        if (m_pGetCompass3DSensorInforFun == NULL)
            return false;

        return m_pGetCompass3DSensorInforFun(pInforArray);
    }

    bool GetAmbientLightInfor(OUT SAmbientLightInforArray* pInforArray)
    {
        if (m_pGetAmbientLightSensorInforFun == NULL)
            return false;

        return m_pGetAmbientLightSensorInforFun(pInforArray);
    }

    bool GetGpsInfor(OUT SGpsInforArray* pInforArray)
    {
        if (m_pGetGpsSensorInforFun == NULL)
            return false;

        return m_pGetGpsSensorInforFun(pInforArray);
    }

private:
    HMODULE m_hSensorObject; ///< SensorObject DLL句柄

    GetAccelerometer3DSensorInforFun m_pGetAccelerometer3DSensorInforFun; ///< 获取重力加速度传感器信息函数指针
    GetGyrometer3DSensorInforFun m_pGetGyrometer3DSensorInforFun; ///< 获取陀螺仪传感器信息函数指针
    GetCompass3DSensorInforFun m_pGetCompass3DSensorInforFun; ///< 获取指南针传感器信息函数指针
    GetAmbientLightSensorInforFun m_pGetAmbientLightSensorInforFun; ///< 获取光传感器函数指针
    GetGpsSensorInforFun m_pGetGpsSensorInforFun; ///< 获取GPS传感器函数指针
};

LSensorObject::LSensorObject()
{
   m_pSensorObject = NULL;

   m_pSensorObject = new CSensorObject();
}

LSensorObject::~LSensorObject()
{
    if (m_pSensorObject != NULL)
    {
        delete m_pSensorObject;
        m_pSensorObject = NULL;
    }
}

bool LSensorObject::GetAccelerometer3DInfor(OUT SAccelerometer3DInforArray* pInforArray)
{
    return m_pSensorObject->GetAccelerometer3DInfor(pInforArray);
}

bool LSensorObject::GetGyrometer3DInfor(OUT SGyrometer3DInforArray* pInforArray)
{
    return m_pSensorObject->GetGyrometer3DInfor(pInforArray);
}

bool LSensorObject::GetCompass3DInfor(OUT SCompass3DInforArray* pInforArray)
{
    return m_pSensorObject->GetCompass3DInfor(pInforArray);
}

bool LSensorObject::GetAmbientLightInfor(OUT SAmbientLightInforArray* pInforArray)
{
    return m_pSensorObject->GetAmbientLightInfor(pInforArray);
}

bool LSensorObject::GetGpsInfor(OUT SGpsInforArray* pInforArray)
{
    return m_pSensorObject->GetGpsInfor(pInforArray);
}