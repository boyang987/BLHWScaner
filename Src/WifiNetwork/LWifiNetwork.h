

#ifndef _LWIFINetwork_H_
#define _LWIFINetwork_H_

#include <string>
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


class CWifiNetwork;

/// <SUMMARY>
/// WIFI网络类
/// </SUMMARY>
class LWifiNetwork
{
public:
    LWifiNetwork();
    ~LWifiNetwork();

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
    /// strGUID
    /// </PARAM>
    /// <RETURNS>
    /// 成功获取返回true, 获取失返回false
    /// </RETURNS>
    bool GetWLANGUID(OUT wstring& strGUID);

private:
    CWifiNetwork* m_pWifiNetwork;
};

#endif