

#ifndef _LWMICOREMANAGER_H_
#define _LWMICOREMANAGER_H_



#include <string>
using std::wstring;
#include <vector>
using std::vector;

#ifndef NAMESPACE_ROOT_WMI
#define NAMESPACE_ROOT_WMI L"ROOT\\WMI"
#endif

#ifndef NAMESPACE_ROOT_CIMV2
#define NAMESPACE_ROOT_CIMV2 L"ROOT\\cimv2"
#endif


typedef unsigned char LBYTE;
typedef unsigned short LUINT16;
typedef unsigned int LUINT;
typedef unsigned __int64 LUINT64;

struct IWbemLocator;
struct IWbemServices;
struct IEnumWbemClassObject;
struct IWbemClassObject;
struct IWbemRefresher;
struct IWbemConfigureRefresher;

namespace LWMI
{
    class LInitCom;

    /// @brief WMI核心对象
    class LWMICoreManager
    {
    public:
        LWMICoreManager();
        ~LWMICoreManager();

    public:
        /// @brief 初始化
        /// @param[in] pNamespace 需要连接的名字空间
        /// @return 成功返回true, 失败返回false
        bool BaseInit(const wchar_t* pNamespace);

        /// @brief WQL查询
        /// @param[in] pQuery 查询语句
        /// @return 成功返回true, 失败返回false
        bool WQLQuery(const wchar_t* pQuery);

        /// @brief 获取对象数量
        /// @return 对象的数量
        int GetObjectsCount();

        /// @brief 获取字符串属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] strProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetStringProperty(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty);

        /// @brief 获取字符串属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] strProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetStringPropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui8Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT8Property(int objectIndex, const wchar_t* pPropertyName, LBYTE& ui8Property);

        /// @brief 获取字节数组属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] arrayProperty 存储字节数组属性
        /// @return 成功返回true, 失败返回false
        bool GetUINT8ArrayProperty(int objectIndex, const wchar_t* pPropertyName, vector<LBYTE>& arrayProperty);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT16Property(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property);

        /// @brief 获取无符号整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT16PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT32Property(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty);

        /// @brief 获取无符号整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT32PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty);

        /// @brief 获取无符号64位整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui64Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT64Property(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property);

        /// @brief 获取无符号64位整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui64Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT64PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property);

    private:
        /// @brief 清理资源
        void BaseCleanUp();

    private:
        IWbemLocator* m_pWbemLocator;
        IWbemServices* m_pWbemServices;

        IEnumWbemClassObject* m_pEnumObject;
        IWbemClassObject** m_pObjectArray;
        int m_objectCount;

        IWbemRefresher* m_pWbemRefresher;
        IWbemConfigureRefresher* m_pWbemConfigRefresher;
        IWbemClassObject** m_pRefreshAbleObjectArray;

        LInitCom* m_pInitComObject;
    };

}


#endif

