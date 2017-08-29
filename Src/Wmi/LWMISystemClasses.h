
#ifndef _LWMI_SYSTEM_CLASSES_H_
#define _LWMI_SYSTEM_CLASSES_H_


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

namespace LWMI
{
    class LWMICoreManager;

    /// @brief 计算机系统管理者类(非操作系统概念上应该包含软硬件)
    class LComputerSystemManager
    {
    public:
        LComputerSystemManager();
        ~LComputerSystemManager();

        /// @brief 获取计算机类型
        enum LCOMPUTER_SYSTEM_TYPE
        {
            COMPUTER_SYSTEM_UNKNOWN = 0, ///< 未知
            COMPUTER_SYSTEM_DESKTOP = 1, ///< 台式机
            COMPUTER_SYSTEM_NOTE_BOOK = 2, ///< 笔记本
            COMPUTER_SYSTEM_TABLET = 3 ///< 平板电脑
        };

        /// @brief 获取计算机系统数量
        /// @return 计算机系统数量(应该为1)
        int GetComputerSystemCount();

        /// @brief 获取计算机系统型号
        /// @param[in] index 索引
        /// @param[out] model 存储型号
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemModel(IN int index, OUT wstring& model);

        /// @brief 获取计算机系统制造商
        /// @param[in] index 索引
        /// @param[out] manufacturer 存储制造商
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取计算机类型
        /// @param[in] index 索引
        /// @param[out] type 存储类型
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemPCType(IN int index, OUT LCOMPUTER_SYSTEM_TYPE& type);

        /// @brief 获取计算机系统类型
        ///
        /// 如: "x64-based PC"
        /// @param[in] index 索引
        /// @param[out] type 存储类型
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemType(IN int index, OUT wstring& type);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LComputerSystemManager(const LComputerSystemManager&);
        LComputerSystemManager& operator = (const LComputerSystemManager&);
    };

    /// @brief 操作系统管理者类
    class LOperatingSystemManager
    {
    public:
        LOperatingSystemManager();
        ~LOperatingSystemManager();

        /// @brief 获取操作系统数量
        /// @return 操作系统数量(应该为1)
        int GetOSCount();

        /// @brief 获取操作系统标题
        /// @param[in] index 索引
        /// @param[out] caption 标题
        /// @return 成功返回true, 失败返回false
        bool GetOSCaption(IN int index, OUT wstring& caption);

        /// @brief 获取操作系统架构
        /// @param[in] index 索引
        /// @param[out] architecture 操作系统架构
        /// @return 成功返回true, 失败返回false
        bool GetOSArchitecture(IN int index, OUT wstring& architecture);

        /// @brief 获取操作系统版本
        /// @param[in] index 索引
        /// @param[out] version 版本
        /// @return 成功返回true, 失败返回false
        bool GetOSVersion(IN int index, OUT wstring& version);

        /// @brief 获取操作系统系统盘符
        /// @param[in] index 索引
        /// @param[out] drive 系统盘符
        /// @return 成功返回true, 失败返回false
        bool GetOSSystemDrive(IN int index, OUT wstring& drive);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LOperatingSystemManager(const LOperatingSystemManager&);
        LOperatingSystemManager& operator = (const LOperatingSystemManager&);
    };

    /// @brief 系统信息管理者类
    class LMS_SystemInformationManager
    {
    public:
        LMS_SystemInformationManager();
        ~LMS_SystemInformationManager();

        /// @brief 获取系统信息数量
        /// @return 系统信息数量(应该为1)
        int GetSystemInforCount();

        /// @brief 获取主板制造商
        /// @param[in] index
        /// @param[out] manufacturer 存储制造商信息
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取主板产品名称
        /// @param[in] index
        /// @param[out] product 存储产品名称
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardProductName(IN int index, OUT wstring& product);

        /// @brief 获取BIOS发布日期
        /// @param[in] index
        /// @param[out] releaseDate 存储发布日期
        /// @return 成功返回true, 失败返回false
        bool GetBIOSReleaseDate(IN int index, OUT wstring& releaseDate);

        /// @brief 获取BIOS厂商信息
        /// @param[in] index
        /// @param[out] vendor 存储厂商信息 
        /// @return 成功返回true, 失败返回false
        bool GetBIOSVendor(IN int index, OUT wstring& vendor);

        /// @brief 获取BIOS版本
        /// @param[in] index 
        /// @param[out] version 存储BIOS版本
        /// @return 成功返回true, 失败返回false
        bool GetBIOSVersion(IN int index, OUT wstring& version);

        /// @brief 获取系统家族
        /// @param[in] index
        /// @param[out] family 存储系统家族
        /// @return 成功返回true, 失败返回false
        bool GetSystemFamily(IN int index, OUT wstring& family);

        /// @brief 获取系统制造商信息
        /// @param[in] index
        /// @param[in] manufacturer 存储制造商信息
        /// @return 成功返回true, 失败返回false
        bool GetSystemManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取系统产品名称
        /// @param[in] index
        /// @param[out] productName 存储产品名称
        /// @return 成功返回true, 失败返回false
        bool GetSystemProductName(IN int index, OUT wstring& productName);

        /// @brief 获取系统SKU信息
        ///
        /// 如: ASUS-NotebookSKU
        /// @param[in] index
        /// @param[in] sku 存储SKU信息
        /// @return 成功返回true, 失败返回false
        bool GetSystemSKU(IN int index, OUT wstring& sku);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMS_SystemInformationManager(const LMS_SystemInformationManager&);
        LMS_SystemInformationManager& operator = (const LMS_SystemInformationManager&);
    };

    /// @brief SMBIOS二进制数据表管理者类
    class LMSSmBios_RawSMBiosTablesManager
    {
    public:
        LMSSmBios_RawSMBiosTablesManager();
        ~LMSSmBios_RawSMBiosTablesManager();

        /// @brief 获取SMBios数据表数目(一般为1)
        /// @return SMBios数据表数目
        int GetRawSMBiosTablesCount();

        /// @brief 获取SMBios主版本
        /// @param[in] index 索引
        /// @param[out] version 存储版本
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosMajorVersion(IN int index, OUT unsigned char& version);

        /// @brief 获取SMBios次版本
        /// @param[in] index 索引
        /// @param[out] version 存储版本
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosMinorVersion(IN int index, OUT unsigned char& version);

        /// @brief 获取SMBios数据
        /// @param[in] index 索引
        /// @param[out] data 存储数据
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosData(IN int index, OUT vector<unsigned char>& data);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMSSmBios_RawSMBiosTablesManager(const LMSSmBios_RawSMBiosTablesManager&);
        LMSSmBios_RawSMBiosTablesManager& operator = (const LMSSmBios_RawSMBiosTablesManager&);
    };

    /// @brief 系统信息管理者类
    class LPerfRawData_PerfOS_MemoryManager
    {
    public:
        LPerfRawData_PerfOS_MemoryManager();
        ~LPerfRawData_PerfOS_MemoryManager();

        /// @brief 获取内存性能数据数量
        /// @return 内存性能数据数量(应该为1)
        int GetMemoryPerfDataCount();

        /// @brief 获取内存可用大小
        /// @param[in] index 索引
        /// @param[out] availableBytes 可用内存大小, 单位M
        /// @return 成功返回true, 失败返回false
        bool GetMemoryAvailableMBytes(IN int index, OUT unsigned long& availableBytes);

        /// @brief 获取内存未使用大小
        /// @param[in] index 索引
        /// @param[out] unusedBytes 可用内存大小, 单位M
        /// @return 成功返回true, 失败返回false
        bool GetMemoryUnusedMBytes(IN int index, OUT unsigned long& unusedBytes);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LPerfRawData_PerfOS_MemoryManager(const LPerfRawData_PerfOS_MemoryManager&);
        LPerfRawData_PerfOS_MemoryManager& operator = (const LPerfRawData_PerfOS_MemoryManager&);
    };
}

#endif