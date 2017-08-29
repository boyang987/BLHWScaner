
#include "LWifiNetwork.h"



#include <string>
using std::string;
using std::wstring;
#include <sstream>
using std::stringstream;
#include <vector>
using std::vector;

#include <windows.h>
#include <wlanapi.h>

#pragma comment(lib, "Wlanapi.lib")
#pragma comment(lib, "Rpcrt4.lib")

/// <SUMMARY>
/// WIFI结构
/// </SUMMARY>
struct SWifiNetwork
{
    string SSID; // 网络名称
    bool IsCurrentConnected; // 是否为当前连接的网络
};


/// <SUMMARY>
/// WIFI网络实现类
/// </SUMMARY>
class CWifiNetwork
{
public:
    CWifiNetwork();
    ~CWifiNetwork();

    /// <SUMMARY>
    /// WLAN是否存在
    /// </SUMMARY>
    /// <RETURNS>
    /// 存在返回true, 不存在返回false
    /// </RETURNS>
    bool IsWLANExist();

    /// <SUMMARY>
    /// 获取WLAN描述
    /// </SUMMARY>
    /// <PARAM name = "desc" dir = "OUT">
    /// 描述
    /// </PARAM>
    /// <RETURNS>
    ///  成功获取返回true, 获取失返回false
    /// </RETURNS>
    bool GetWLANDesc(OUT wstring& desc);

    /// <SUMMARY>
    /// 获取WLAN GUID
    /// </SUMMARY>
    /// <PARAM name = "guid" dir = "OUT">
    /// GUID
    /// </PARAM>
    /// <RETURNS>
    /// 成功获取返回true, 获取失返回false
    /// </RETURNS>
    bool GetWLANGUID(OUT GUID& guid);

    /// <SUMMARY>
    /// 判断硬件Wifi开关是否被开启
    /// </SUMMARY>
    /// <RETURNS>
    /// 已经开启返回true, 没有开启返回false
    /// </RETURNS>
    bool HardwareWLANIsEnabled();

    /// <SUMMARY>
    /// 判断软件Wifi开关是否被开启
    /// </SUMMARY>
    /// <RETURNS>
    /// 已经开启返回true, 没有开启返回false
    /// </RETURNS>
    bool SoftwareWLANIsEnabled();

    /// <SUMMARY>
    /// 软体开启Wifi
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功开启返回true, 开启失败返回false
    /// </RETURNS>
    bool SoftwareEnableWLAN();

    /// <SUMMARY>
    /// 软体关闭Wifi
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功关闭返回true, 关闭失败返回false
    /// </RETURNS>
    bool SoftwareDisableWLAN();

    /// <SUMMARY>
    /// 判断WLAN是否连接上无线网络
    /// </SUMMARY>
    /// <RETURNS>
    /// 已经连接返回true, 没有连接返回false
    /// </RETURNS>
    bool WLANIsConnected();

    /// <SUMMARY>
    /// 判断WLAN是否连接上无线网络
    /// </SUMMARY>
    /// <PARAM name = "ssid" dir = "OUT">
    /// 保存网络SSID(wifi名称)
    /// </PARAM>
    /// <RETURNS>
    /// 已经连接返回true, 没有连接返回false
    /// </RETURNS>
    bool WLANIsConnected(OUT string& ssid);

    /// <SUMMARY>
    /// 断开无线网络连接
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功返回true, 失败返回false
    /// </RETURNS>
    bool WLANDisconnect();

    /// <SUMMARY>
    /// 连接到指定网络
    /// </SUMMARY>
    /// <PARAM name = "ssid" dir = "IN">
    /// 网络SSID(wifi名称)
    /// </PARAM>
    /// <PARAM name = "key" dir = "IN">
    /// 网络密钥(wifi密码)
    /// </PARAM>
    /// <RETURNS>
    /// 成功获取返回true, 获取失返回false, 可使用GetErrorMessage获取错误信息
    /// </RETURNS>
    bool WLANConnect(IN const string& ssid, IN const string& key);

    /// <SUMMARY>
    /// 连接到指定网络
    /// 要求系统中已经存在该网络连接的配置文件
    /// </SUMMARY>
    /// <PARAM name = "ssid" dir = "IN">
    /// 网络SSID(wifi名称)
    /// </PARAM>
    /// <RETURNS>
    /// 成功获取返回true, 获取失返回false, 可使用GetErrorMessage获取错误信息
    /// </RETURNS>
    bool WLANConnect(IN const string& ssid);

    /// <SUMMARY>
    /// 获取可用网络列表
    /// </SUMMARY>
    /// <PARAM name = "networkist" dir = "OUT">
    /// 网络列表
    /// </PARAM>
    /// <RETURNS>
    ///  成功获取返回true, 获取失返回false, 可使用GetErrorMessage获取错误信息
    /// </RETURNS>
    bool GetAvailableNetworkList(OUT vector<SWifiNetwork>& networkist);

    /// <SUMMARY>
    /// 获取错误信息
    /// </SUMMARY>
    /// <RETURNS>
    /// 错误信息
    /// </RETURNS>
    string GetErrorMessage();

private:
    /// <SUMMARY>
    /// WIFI网络配置文件结构
    /// </SUMMARY>
    struct SWifiProfile
    {
        string SSID; // 网络SSID
        string Key; // 网络密钥
        DOT11_AUTH_ALGORITHM AuthAlgo; // 认证算法
        DOT11_CIPHER_ALGORITHM CipherAlgo; // 加密算法
    };    

    /// <SUMMARY>
    /// 初始化
    /// </SUMMARY>
    /// <RETURNS>
    /// 初始化成功返回true, 失败返回false, 可使用GetErrorMessage获取错误信息
    /// </RETURNS>
    bool Init();

    /// <SUMMARY>
    /// 清理资源
    /// </SUMMARY>
    void UnInit();

    void CreateWifiProfile(const SWifiProfile& wifiProfile, OUT wstring& profileStr);

private:
    bool m_bWLANExist; // 标识WLAN是否存在
    bool m_bInitSuccess; // 标识是否已经初始化成功
    HANDLE m_hWLAN; // 无线网卡句柄
    GUID m_guidWLAN; // 无线网卡接口GUID
    wstring m_descWLAN; // 无线网卡接口描述
    stringstream m_errStream; // 错误处理流
};

CWifiNetwork::CWifiNetwork()
{
    m_hWLAN = NULL;
    m_bInitSuccess = false;
    m_bWLANExist = false;

    if (this->Init())
        m_bInitSuccess = true;
    else
        m_bInitSuccess = false;
};

CWifiNetwork::~CWifiNetwork()
{
    this->UnInit();
}


string CWifiNetwork::GetErrorMessage()
{
    return m_errStream.str();
}

bool CWifiNetwork::Init()
{
    bool bReturn = false;

    DWORD dwRet;
    DWORD version;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;

    m_bInitSuccess = false;

    this->UnInit();

    dwRet = WlanOpenHandle(2, NULL, &version, &m_hWLAN);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanOpenHandle FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

  
    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    bReturn = true;
    m_bInitSuccess = true;

    if (pWLANInterfaceList->dwNumberOfItems > 0)
    {
        m_guidWLAN = pWLANInterfaceList->InterfaceInfo[0].InterfaceGuid;
        m_descWLAN = pWLANInterfaceList->InterfaceInfo[0].strInterfaceDescription;
        m_bWLANExist = true;
    }
    else
    {
        m_bWLANExist = false;
    }

SAFE_EXIT:
    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }

    return bReturn;
}

bool CWifiNetwork::IsWLANExist()
{
    if (m_bInitSuccess && m_bWLANExist)
        return true;

    return false;
}

bool CWifiNetwork::GetWLANDesc(OUT wstring& desc)
{
    if (!m_bInitSuccess)
        return false;

    if (!m_bWLANExist)
        return false;

    desc = m_descWLAN;

    return true;
}

bool CWifiNetwork::GetWLANGUID(OUT GUID& guid)
{
    if (!m_bInitSuccess)
        return false;

    if (!m_bWLANExist)
        return false;

    guid = m_guidWLAN;

    return true;
}

bool CWifiNetwork::HardwareWLANIsEnabled()
{
    bool bReturn = false;

    DWORD dwRet;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;
    PWLAN_RADIO_STATE pWLANRadioState = NULL;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANInterfaceList->dwNumberOfItems < 1)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    
    DWORD dwDataSize = sizeof(WLAN_RADIO_STATE);
    dwRet = WlanQueryInterface(
        m_hWLAN, 
        &(pWLANInterfaceList->InterfaceInfo[0].InterfaceGuid),
        wlan_intf_opcode_radio_state,
        NULL,
        &dwDataSize,
        (PVOID*)&pWLANRadioState,
        NULL
        );
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanQueryInterface FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANRadioState->PhyRadioState[0].dot11HardwareRadioState == dot11_radio_state_on)
    {
        bReturn = true;
        goto SAFE_EXIT;
    }

SAFE_EXIT:

    if (pWLANRadioState != NULL)
    {
        WlanFreeMemory(pWLANRadioState);
        pWLANRadioState = NULL;
    }

    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::SoftwareWLANIsEnabled()
{
    bool bReturn = false;

    DWORD dwRet;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;
    PWLAN_RADIO_STATE pWLANRadioState = NULL;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANInterfaceList->dwNumberOfItems < 1)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }


    DWORD dwDataSize = sizeof(WLAN_RADIO_STATE);
    dwRet = WlanQueryInterface(
        m_hWLAN, 
        &(pWLANInterfaceList->InterfaceInfo[0].InterfaceGuid),
        wlan_intf_opcode_radio_state,
        NULL,
        &dwDataSize,
        (PVOID*)&pWLANRadioState,
        NULL
        );
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanQueryInterface FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANRadioState->PhyRadioState[0].dot11SoftwareRadioState == dot11_radio_state_on)
    {
        bReturn = true;
        goto SAFE_EXIT;
    }

SAFE_EXIT:

    if (pWLANRadioState != NULL)
    {
        WlanFreeMemory(pWLANRadioState);
        pWLANRadioState = NULL;
    }

    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::SoftwareEnableWLAN()
{
    bool bReturn = false;

    DWORD dwRet;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;
    PWLAN_RADIO_STATE pWLANRadioState = NULL;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANInterfaceList->dwNumberOfItems < 1)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    WLAN_PHY_RADIO_STATE radioState;
    radioState.dwPhyIndex = 0;
    radioState.dot11SoftwareRadioState = dot11_radio_state_on;
    PVOID pData = &radioState;

    dwRet = WlanSetInterface(
        m_hWLAN,
        &(pWLANInterfaceList->InterfaceInfo[0].InterfaceGuid),
        wlan_intf_opcode_radio_state,
        sizeof(WLAN_PHY_RADIO_STATE),
        pData,
        NULL
        );

    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanSetInterface FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    bReturn = TRUE;

SAFE_EXIT:

    if (pWLANRadioState != NULL)
    {
        WlanFreeMemory(pWLANRadioState);
        pWLANRadioState = NULL;
    }

    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::SoftwareDisableWLAN()
{
    bool bReturn = false;

    DWORD dwRet;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;
    PWLAN_RADIO_STATE pWLANRadioState = NULL;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANInterfaceList->dwNumberOfItems < 1)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    WLAN_PHY_RADIO_STATE radioState;
    radioState.dwPhyIndex = 0;
    radioState.dot11SoftwareRadioState = dot11_radio_state_off;
    PVOID pData = &radioState;

    dwRet = WlanSetInterface(
        m_hWLAN,
        &(pWLANInterfaceList->InterfaceInfo[0].InterfaceGuid),
        wlan_intf_opcode_radio_state,
        sizeof(WLAN_PHY_RADIO_STATE),
        pData,
        NULL
        );

    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanSetInterface FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    bReturn = TRUE;

SAFE_EXIT:

    if (pWLANRadioState != NULL)
    {
        WlanFreeMemory(pWLANRadioState);
        pWLANRadioState = NULL;
    }

    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::WLANIsConnected()
{
    bool bReturn = false;

    DWORD dwRet;
    PWLAN_INTERFACE_INFO_LIST pWLANInterfaceList = NULL;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    dwRet = WlanEnumInterfaces(m_hWLAN, NULL, &pWLANInterfaceList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanEnumInterfaces FAIL, Error code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (pWLANInterfaceList->dwNumberOfItems > 0)
    {
        if (pWLANInterfaceList->InterfaceInfo[0].isState == wlan_interface_state_connected)
            bReturn = true;
        else
            bReturn = false;

        goto SAFE_EXIT;
    }
    else
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

SAFE_EXIT:
    if (pWLANInterfaceList != NULL)
    {
        WlanFreeMemory(pWLANInterfaceList);
        pWLANInterfaceList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::WLANIsConnected(OUT string& ssid)
{
	bool bRet = false;
	vector<SWifiNetwork> networkList;
	this->GetAvailableNetworkList(networkList);
	for (unsigned int i = 0; i < networkList.size(); i++)
	{
		if (networkList[i].IsCurrentConnected)
		{
			ssid = networkList[i].SSID;
			bRet = true;
			break;
		}
	}

	return bRet;
}

bool CWifiNetwork::WLANDisconnect()
{
    if (!m_bInitSuccess)
        return false;

    if (!m_bWLANExist)
        return false;

    DWORD dwRet = WlanDisconnect(m_hWLAN, &m_guidWLAN, NULL);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanDisconnect FAIL, Error code: " << dwRet;
        return false;
    }

    return true;
}

bool CWifiNetwork::WLANConnect(IN const string& ssid, IN const string& key)
{
    bool bReturn = false;
    WLAN_REASON_CODE reasonCode;
    DWORD dwRet;
    int destNetworkIndex = -1;
    PWLAN_AVAILABLE_NETWORK_LIST pNetworkList = NULL;
    WLAN_CONNECTION_PARAMETERS connParam;

    SWifiProfile wifiProfile;
    wstring profileStr;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    // 获取网络列表
    dwRet = WlanGetAvailableNetworkList(m_hWLAN, &m_guidWLAN, 
        WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES, NULL, &pNetworkList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanGetAvailableNetworkList FAIL, Error Code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }


    // 搜索指定网络
    for (unsigned int i = 0; i < pNetworkList->dwNumberOfItems; i++ )
    {
        if (ssid.compare((char*)(pNetworkList->Network[i].dot11Ssid.ucSSID)) == 0)
        {
            destNetworkIndex = (int)i;
            break;
        }
    }

    // 没有找到指定的网络
    if (destNetworkIndex == -1)
    {
        m_errStream.str("");
        m_errStream << "Can't find specific wifi";
        bReturn = false;
        goto SAFE_EXIT;
    }

    // 指定网络已经被连接
    if ((pNetworkList->Network[destNetworkIndex].dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) ==
        WLAN_AVAILABLE_NETWORK_CONNECTED)
    {
        bReturn = true;
        goto SAFE_EXIT;
    }

    // 创建网络配置文件
    wifiProfile.SSID = ssid;
    wifiProfile.Key = key;
    wifiProfile.AuthAlgo = pNetworkList->Network[destNetworkIndex].dot11DefaultAuthAlgorithm;
    wifiProfile.CipherAlgo = pNetworkList->Network[destNetworkIndex].dot11DefaultCipherAlgorithm;
    this->CreateWifiProfile(wifiProfile, profileStr);

    // 设置网络配置文件
    dwRet = WlanSetProfile(m_hWLAN, &(m_guidWLAN), 0, profileStr.c_str(), NULL, TRUE, NULL, &reasonCode);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanSetProfile FAIL, Error code: " << dwRet << "Reason code: " << reasonCode << std::endl;
        m_errStream << profileStr.c_str();

        bReturn = false;
        goto SAFE_EXIT;
    }

    connParam.wlanConnectionMode = wlan_connection_mode_profile;
    connParam.strProfile =pNetworkList->Network[destNetworkIndex].strProfileName;								
    connParam.pDot11Ssid = NULL;
    connParam.pDesiredBssidList = NULL;
    connParam.dwFlags = 0;
    connParam.dot11BssType = dot11_BSS_type_infrastructure;
    dwRet = WlanConnect(m_hWLAN, &(m_guidWLAN), &connParam, NULL);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanConnect FAIL, Error code: " << dwRet;

        bReturn = false;
        goto SAFE_EXIT;
    }

    bReturn = true;


SAFE_EXIT:
    if (pNetworkList != NULL)
    {
        WlanFreeMemory(pNetworkList);
        pNetworkList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::WLANConnect(IN const string& ssid)
{
    bool bReturn = false;
    DWORD dwRet;
    int destNetworkIndex = -1;
    PWLAN_AVAILABLE_NETWORK_LIST pNetworkList = NULL;
    WLAN_CONNECTION_PARAMETERS connParam;

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    // 获取网络列表
    dwRet = WlanGetAvailableNetworkList(m_hWLAN, &m_guidWLAN, 
        WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES, NULL, &pNetworkList);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanGetAvailableNetworkList FAIL, Error Code: " << dwRet;
        bReturn = false;
        goto SAFE_EXIT;
    }


    // 搜索指定网络
    for (unsigned int i = 0; i < pNetworkList->dwNumberOfItems; i++ )
    {
        if (ssid.compare((char*)(pNetworkList->Network[i].dot11Ssid.ucSSID)) == 0)
        {
            destNetworkIndex = (int)i;
            break;
        }
    }

    // 没有找到指定的网络
    if (destNetworkIndex == -1)
    {
        m_errStream.str("");
        m_errStream << "Can't find specific wifi";
        bReturn = false;
        goto SAFE_EXIT;
    }

    // 指定网络已经被连接
    if ((pNetworkList->Network[destNetworkIndex].dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) ==
        WLAN_AVAILABLE_NETWORK_CONNECTED)
    {
        bReturn = true;
        goto SAFE_EXIT;
    }

    connParam.wlanConnectionMode = wlan_connection_mode_profile;
    connParam.strProfile =pNetworkList->Network[destNetworkIndex].strProfileName;								
    connParam.pDot11Ssid = NULL;
    connParam.pDesiredBssidList = NULL;
    connParam.dwFlags = 0;
    connParam.dot11BssType = dot11_BSS_type_infrastructure;
    dwRet = WlanConnect(m_hWLAN, &(m_guidWLAN), &connParam, NULL);
    if (dwRet != ERROR_SUCCESS)
    {
        m_errStream.str("");
        m_errStream << "WlanConnect FAIL, Error code: " << dwRet;

        bReturn = false;
        goto SAFE_EXIT;
    }

    bReturn = true;


SAFE_EXIT:
    if (pNetworkList != NULL)
    {
        WlanFreeMemory(pNetworkList);
        pNetworkList = NULL;
    }
    return bReturn;
}

bool CWifiNetwork::GetAvailableNetworkList(OUT vector<SWifiNetwork>& networkist)
{
    bool bReturn = false;
    SWifiNetwork network;
    DWORD dwRet;
    PWLAN_AVAILABLE_NETWORK_LIST pNetworkList = NULL;

    networkist.clear();

    if (!m_bInitSuccess)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

    if (!m_bWLANExist)
    {
        bReturn = false;
        goto SAFE_EXIT;
    }

   dwRet = WlanGetAvailableNetworkList(m_hWLAN, &m_guidWLAN, 
       WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES, NULL, &pNetworkList);
   if (dwRet != ERROR_SUCCESS)
   {
       m_errStream.str("");
       m_errStream << "WlanGetAvailableNetworkList FAIL, Error Code: " << dwRet;
       bReturn = false;
       goto SAFE_EXIT;
   }

   bReturn = true;

   for (unsigned int i = 0; i < pNetworkList->dwNumberOfItems; i++ )
   {
       network.IsCurrentConnected = false;
       network.SSID = (char*)(pNetworkList->Network[i].dot11Ssid.ucSSID);
       if ((pNetworkList->Network[i].dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) ==
           WLAN_AVAILABLE_NETWORK_CONNECTED)
       {
           network.IsCurrentConnected = true;
       }

       networkist.push_back(network);
   }


SAFE_EXIT:
    if (pNetworkList != NULL)
    {
        WlanFreeMemory(pNetworkList);
        pNetworkList = NULL;
    }
    return bReturn;
}

void CWifiNetwork::UnInit()
{
    if (m_hWLAN != NULL)
    {
        WlanCloseHandle(m_hWLAN, NULL);
        m_hWLAN = NULL;
    }
}


void CWifiNetwork::CreateWifiProfile(const SWifiProfile& wifiProfile, OUT wstring& profileStr)
{
    string authAlgo;
    switch (wifiProfile.AuthAlgo)
    {
    case DOT11_AUTH_ALGO_80211_OPEN:
    case DOT11_AUTH_ALGO_IHV_START:
    case DOT11_AUTH_ALGO_IHV_END:
        authAlgo = "open";
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        authAlgo = "shared";
        break;
    case DOT11_AUTH_ALGO_WPA:
    case DOT11_AUTH_ALGO_WPA_NONE:
        authAlgo = "WPA";
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        authAlgo = "WPAPSK";
        break;
    case DOT11_AUTH_ALGO_RSNA:
        authAlgo = "WPA2";
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        authAlgo = "WPA2PSK";
        break;
    default:
        authAlgo = "open";
        break;
    }

    string cipherAlgo;
    switch (wifiProfile.CipherAlgo)
    {
    case DOT11_CIPHER_ALGO_NONE:
    case DOT11_CIPHER_ALGO_IHV_START:
    case DOT11_CIPHER_ALGO_IHV_END:
        cipherAlgo = "none";
        break;
    case  DOT11_CIPHER_ALGO_WEP40:
    case DOT11_CIPHER_ALGO_WEP:
        cipherAlgo = "WEP";
        break;
    case DOT11_CIPHER_ALGO_CCMP:
    case DOT11_CIPHER_ALGO_WEP104:
    case DOT11_CIPHER_ALGO_WPA_USE_GROUP:
    //case DOT11_CIPHER_ALGO_RSN_USE_GROUP:
        cipherAlgo = "AES";
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        cipherAlgo = "TKIP";
        break;
    default:
        cipherAlgo = "none";
        break;
    }

    string keyType;
    if (cipherAlgo.compare("WEP") == 0)
        keyType = "networkKey";
    else
        keyType = "passPhrase";


    const int bufferLen = 2048; 
    wchar_t profileBuffer[bufferLen] = {0};

    swprintf_s(profileBuffer, bufferLen,
                   L"<?xml version=\"1.0\" encoding=\"US-ASCII\"?> \
                      <WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"> \
                      <name>%S</name> \
                      <SSIDConfig> \
                      <SSID> \
                      <name>%S</name> \
                      </SSID> \
                      </SSIDConfig> \
                      <connectionType>ESS</connectionType> \
                      <connectionMode>auto</connectionMode> \
                      <autoSwitch>false</autoSwitch> \
                      <MSM> \
                      <security> \
                      <authEncryption> \
                      <authentication>%S</authentication> \
                      <encryption>%S</encryption> \
                      <useOneX>false</useOneX> \
                      </authEncryption> \
                      <sharedKey> \
                      <keyType>%S</keyType> \
                      <protected>false</protected> \
                      <keyMaterial>%S</keyMaterial> \
                      </sharedKey> \
                      </security> \
                      </MSM> \
                      </WLANProfile>",
                      wifiProfile.SSID.c_str(),
                      wifiProfile.SSID.c_str(),
                      authAlgo.c_str(),
                      cipherAlgo.c_str(),
                      keyType.c_str(),
                      wifiProfile.Key.c_str());

    profileStr = profileBuffer;
}

LWifiNetwork::LWifiNetwork()
{
    m_pWifiNetwork = new CWifiNetwork();
}

LWifiNetwork::~LWifiNetwork()
{
    delete m_pWifiNetwork;
    m_pWifiNetwork = 0;
}

bool LWifiNetwork::IsWLANExist()
{
    return m_pWifiNetwork->IsWLANExist();
}

bool LWifiNetwork::GetWLANDesc(OUT wstring& desc)
{
    return m_pWifiNetwork->GetWLANDesc(desc);
}

bool LWifiNetwork::GetWLANGUID(OUT wstring& strGUID)
{
    GUID guid;
    bool bRet = m_pWifiNetwork->GetWLANGUID(guid);
    if (bRet)
    {
        strGUID = L"{";
        RPC_WSTR pStrGUID = 0;
        UuidToStringW(&guid, &pStrGUID);
        strGUID += (wchar_t*)pStrGUID;
        RpcStringFreeW(&pStrGUID);
        pStrGUID = 0;
        strGUID += L"}";
    }

    return bRet;
}

