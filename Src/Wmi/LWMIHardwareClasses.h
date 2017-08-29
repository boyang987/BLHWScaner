

#ifndef _LWMIHARDWARECLASSES_H_
#define _LWMIHARDWARECLASSES_H_

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

namespace LWMI
{
    class LWMICoreManager;

    /// @brief 主板管理者类
    class LBaseBoardManager
    {
    public:
        LBaseBoardManager();
        ~LBaseBoardManager();

        /// @brief 获取主板数量
        /// @return 主板数量(应该为1)
        int GetBaseBoardCount();

        /// @brief 获取主板标题
        /// @param[in] index 主板索引
        /// @param[out] caption 存储主板标题
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardCaption(IN int index, OUT wstring& caption);

        /// @brief 获取主板描述
        /// @param[in] index 主板索引
        /// @param[out] caption 存储主板描述
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardDescription(IN int index, OUT wstring& description);

        /// @brief 获取主板制造商
        /// @param[in] index 主板索引
        /// @param[out] manufacturer 存储主板制造商
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取主板序列号
        /// @param[in] index 主板索引
        /// @param[存储序列号] serialNumber
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardSerialNumber(IN int index, OUT wstring& serialNumber);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LBaseBoardManager(const LBaseBoardManager&);
        LBaseBoardManager& operator = (const LBaseBoardManager&);
    };

    /// @brief BIOS管理者类
    class LBIOSManager
    {
    public:
        LBIOSManager();
        ~LBIOSManager();

        /// @brief 获取BIOS数量
        /// @return BIOS数量(应该为1)
        int GetBIOSCount();

        /// @brief 获取BIOS描述
        /// @param[in] index BIOS索引
        /// @param[out] description 存储BIOS描述
        /// @return 成功返回true, 失败返回false
        bool GetBIOSDescription(IN int index, OUT wstring& description);

        /// @brief 获取BIOS制造商
        /// @param[in] index BIOS索引
        /// @param[out] manufacturer 存储BIOS制造商
        /// @return 成功返回true, 失败返回false
        bool GetBIOSManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取BIOS序列号
        /// @param[in] index BIOS索引
        /// @param[out] serialNumber 存储BIOS序列号
        /// @return 成功返回true, 失败返回false
        bool GetBIOSSerialNumber(IN int index, OUT wstring& serialNumber);

        /// @brief 获取BIOS版本
        /// @param[in] index BIOS索引
        /// @param[in] version 存储BIOS版本
        /// @return 成功返回true, 失败返回false
        bool GetBIOSVersion(IN int index, OUT wstring& version);

        /// @brief 获取SMBIOS BIOS版本
        /// @param[in] index BIOS索引
        /// @param[in] version 存储SMBIOS BIOS版本
        /// @return 成功返回true, 失败返回false
        bool GetSMBIOSBIOSVersion(IN int index, OUT wstring& version);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LBIOSManager(const LBIOSManager&);
        LBIOSManager& operator = (const LBIOSManager&);
    };

    /// @brief 处理器管理者类
    class LProcessorManager
    {
    public:
        LProcessorManager();
        ~LProcessorManager();

        /// @brief 处理器架构
        enum LPROCESSOR_ARCHITECTURE
        {
            X86 = 0,
            MIPS = 1,
            ALPHA = 2,
            POWER_PC = 3,
            ARM = 5,
            IA64 = 6,
            X64 = 9
        };

        /// @brief 获取处理器数量
        /// @return 处理器数量(一般为1)
        int GetProcessorCount();

        /// @brief 获取处理器名称
        /// @param[in] index 处理器索引
        /// @param[out] name 存储名称
        /// @return 成功返回true, 失败返回false
        bool GetProcessorName(IN int index, OUT wstring& name);

        /// @brief 获取处理器描述
        /// @param[in] index 处理器索引
        /// @param[out] description 存储描述信息
        /// @return 成功返回true, 失败返回false
        bool GetProcessorDesription(IN int index, OUT wstring& description);

        /// @brief 获取处理器架构信息
        /// @param[in] index 处理器索引
        /// @param[out] architecture 存储架构信息
        /// @return 成功返回true, 失败返回false
        bool GetProcessorArchitecture(IN int index, OUT LPROCESSOR_ARCHITECTURE& architecture);

        /// @brief 获取处理器制造商
        /// @param[in] index 处理器索引
        /// @param[out] manufacturer 存储制造商
        /// @return 成功返回true, 失败返回false
        bool GetProcessorManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取处理器最大频率(单位MHz)
        /// @param[in] index 处理器索引
        /// @param[in] speed 存储频率
        /// @return 成功返回true, 失败返回false
        bool GetProcessorMaxClockSpeed(IN int index, OUT unsigned long& speed);

        /// @brief 获取处理器核心数
        /// @param[in] index 处理器索引
        /// @param[in] coresNumber 存储核心数
        /// @return 成功返回true, 失败返回false
        bool GetProcessorCoresNumber(IN int index, OUT unsigned long& coresNumber);

        /// @brief 获取处理器逻辑处理器数量(线程数)
        /// @param[in] index 处理器索引
        /// @param[in] logicalProcessorsNumber 逻辑处理器数量
        /// @return 成功返回true, 失败返回false
        bool GetProcessorLogicalProcessorNumber(IN int index, OUT unsigned long& logicalProcessorsNumber);

        /// @brief 获取处理器的使用率
        ///
        /// 在多核处理器上, 该方法返回的是多个核心的平均使用率
        /// @param[in] index 处理器索引
        /// @param[out] percentage 处理器使用率(百分比)
        /// @return 成功返回true, 失败返回false
        bool GetProcessorLoadPercentage(IN int index, OUT unsigned long& percentage);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LProcessorManager(const LProcessorManager&);
        LProcessorManager& operator = (const LProcessorManager&);
    };

    /// @brief 物理内存管理者类
    class LPhysicalMemoryManager
    {
    public:
        LPhysicalMemoryManager();
        ~LPhysicalMemoryManager();

        /// @brief 获取物理内存数量
        /// @return 物理内存数量
        int GetPhysicalMemoryCount();

        /// @brief 获取物理内存描述
        /// @param[in] index 内存索引
        /// @param[out] description 存储描述
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemoryDescription(IN int index, OUT wstring& description);

        /// @brief 获取物理内存制造商
        /// @param[in] index 内存索引
        /// @param[out] manufacturer 存储制造商
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemoryManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取物理内存序列号
        /// @param[in] index 物理内存索引
        /// @param[out] serialNumber 存储序列号
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemorySerialNumber(IN int index, OUT wstring& serialNumber);

        /// @brief 获取物理内存容量(单位M)
        /// @param[in] index 物理内存索引
        /// @param[out] capacity 存储容量
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemoryCapacity(IN int index, OUT unsigned long& capacity);

        /// @brief 获取物理内存频率(单位MHz)
        /// @param[in] index 物理内存索引
        /// @param[out] speed 存储频率
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemorySpeed(IN int index, OUT unsigned long& speed);

        /// @brief 获取物理内存型号
        /// @param[in] index 索引
        /// @param[out] partNumber 存储型号
        /// @return 成功返回true, 失败返回false
        bool GetPhysicalMemoryPartNumber(IN int index, OUT wstring& partNumber);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LPhysicalMemoryManager(const LPhysicalMemoryManager&);
        LPhysicalMemoryManager& operator = (const LPhysicalMemoryManager&);
    };

    /// @brief 视屏控制器(显卡)管理者类
    class LVideoControllerManager
    {
    public:
        LVideoControllerManager();
        LVideoControllerManager(IN const wstring& pnpDeviceID);
        ~LVideoControllerManager();

        /// @brief 获取视屏控制器数量
        /// @return 视屏控制器数量
        int GetVideoControllerCount();

        /// @brief 获取视屏控制器描述
        /// @param[in] index 视屏控制器索引
        /// @param[in] description 存储视屏控制器描述
        /// @return 成功返回true, 失败返回false
        bool GetVideoControllerDescription(IN int index, OUT wstring& description);

        /// @brief 获取视屏控制器适配器记忆体容量(显存容量)(单位M)
        /// @param[in] index 视屏控制器索引
        /// @param[in] ramCapacity 记忆体容量
        /// @return 成功返回true, 失败返回false
        bool GetVideoControllerAdapterRAMCapacity(IN int index, OUT unsigned long& ramCapacity);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LVideoControllerManager(const  LVideoControllerManager&);
        LVideoControllerManager& operator = (const  LVideoControllerManager&);
    };

    /// @brief 磁盘驱动器管理者类
    class LDiskDriveManager
    {
    public:
        LDiskDriveManager();
        ~LDiskDriveManager();

        /// @brief 磁盘类型
        enum LDISK_TYPE
        {
            UNKNOWN_DISK = 0, // 未知类型
            FIXED_DISK = 1, // 固定磁盘(如本地硬盘)
            EXTERNAL_USB_DISK = 2, // 扩展磁盘(如USB移动硬盘)
            VIRTUAL_DISK = 3, // 虚拟硬盘(如VHD)
            REMOVABLE_DISK = 4, // 可移动盘
        };

        /// @brief 获取磁盘驱动器数量
        /// @return 磁盘数量
        int GetDiskCount();

        /// @brief 获取磁盘型号
        /// @param[in] index 磁盘索引
        /// @param[out] model 磁盘型号
        /// @return 成功返回true, 失败返回false
        bool GetDiskModel(IN int index, OUT wstring& model);

        /// @brief 获取磁盘序列号
        ///  
        /// @param[in] index 磁盘索引
        /// @param[out] serialNumber 序列号
        /// @return 成功返回true, 失败返回false
        bool GetDiskSerialNumber(IN int index, OUT wstring& serialNumber);

        /// @brief 获取磁盘大小
        /// @param[in] index 磁盘索引
        /// @param[out] size 存储磁盘大小, 单位(G)
        /// @return 成功返回true, 失败返回false
        bool GetDiskSize(IN int index, OUT unsigned long& size);

        /// @brief 获取磁盘设备ID
        /// @param[in] index 磁盘索引
        /// @param[out] deviceID 存储磁盘设备ID
        /// @return 成功返回true, 失败返回false
        bool GetDiskDeviceID(IN int index, OUT wstring& deviceID);

        /// @brief 获取磁盘实例路径
        /// @param[in] index 磁盘索引
        /// @param[out] pnpDeviceID 存储磁盘实例路径
        /// @return 成功返回true, 失败返回false
        bool GetDiskPNPDeviceID(IN int index, OUT wstring& pnpDeviceID);

        /// @brief 获取磁盘接口类型
        /// @param[in] index 磁盘索引
        /// @param[out] type 磁盘接口类型(SCSI, HDC, IDE, USB, 1394)
        /// @return 成功返回true, 失败返回false
        bool GetDiskInterfaceType(IN int index, OUT wstring& type);

        /// @brief 获取磁盘类型
        /// @param[in] index 磁盘索引
        /// @param[out] type 存储磁盘类型
        /// @return 成功返回true, 失败返回false
        bool GetDiskType(IN int index, OUT LDISK_TYPE& type);

        /// @brief 获取磁盘逻辑分区名称
        ///  多个分区以分号隔开如: C:;D:;E:
        /// @param[in] index 磁盘索引
        /// @param[out] logicalName 存储磁盘逻辑名称
        /// @return 成功返回true, 失败返回false
        bool GetDiskLogicalName(IN int index, OUT wstring& logicalName);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        // 禁止拷贝构造函数和赋值操作符
        LDiskDriveManager(const  LDiskDriveManager&);
        LDiskDriveManager& operator = (const  LDiskDriveManager&);
    };

    /// @brief 电池管理者类
    class LBatteryManager
    {
    public:
        LBatteryManager();
        ~LBatteryManager();

        /// @brief 获取电池数量
        /// @return 电池数量
        int GetBatteryCount();

        /// @brief 获取电池名称
        /// @param[in] index
        /// @param[out] name 存储名称
        /// @return 成功返回true, 失败返回false
        bool GetBatteryName(IN int index, OUT wstring& name);

        /// @brief 获取电池设备ID
        /// @param[in] index
        /// @param[out] deviceID 存储设备ID
        /// @return 成功返回true, 失败返回false
        bool GetBatteryDeviceID(IN int index, OUT wstring& deviceID);

        /// @brief 获取电池设计电压
        /// @param[in] index 
        /// @param[out] voltage 存储设计电压(单位mV)
        /// @return 成功返回true, 失败返回false
        bool GetBatteryDesignVoltage(IN int index, OUT unsigned long& voltage);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LBatteryManager(const LBatteryManager&);
        LBatteryManager& operator = (const LBatteryManager&);
    };

    /// @brief 电池静态数据管理者类
    class LBatteryStaticDataManager
    {
    public:
        LBatteryStaticDataManager();
        ~LBatteryStaticDataManager();

        /// @brief 获取电池数目
        /// @return 电池数目
        int GetBatteryCount();

        /// @brief 获取电池制造商名称
        /// @param[in] index
        /// @param[in] manufacturer 存储制造商名称
        /// @return 成功返回true, 失败返回false
        bool GetBatteryManufacturerName(IN int index, OUT wstring& manufacturer);

        /// @brief 获取电池唯一ID
        /// @param[in] index
        /// @param[out] uniqueID 存储唯一ID
        /// @return 成功返回true, 失败返回false
        bool GetBatteryUniqueID(IN int index, OUT wstring& uniqueID);

        /// @brief 获取电池序列号
        /// @param[in] index 
        /// @param[out] serialNumber 存储序列号
        /// @return 成功返回true, 失败返回false
        bool GetBatterySerialNumber(IN int index, OUT wstring& serialNumber);

        /// @brief 获取电池设计电量
        /// @param[in] index
        /// @param[out] capacity 存储电量值, 单位mWh
        /// @return 成功返回true, 失败返回false
        bool GetBatteryDesignedCapacity(IN int index, OUT unsigned long& capacity);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LBatteryStaticDataManager(const LBatteryStaticDataManager&);
        LBatteryStaticDataManager& operator = (const LBatteryStaticDataManager&);
    };

    /// @brief 电池满电量管理者类
    class LBatteryFullCapacityManager
    {
    public:
        LBatteryFullCapacityManager();
        ~LBatteryFullCapacityManager();

        /// @brief 获取电池数量
        /// @return 电池数量
        int GetBatteryCount();

        /// @brief 获取电池满电量容量
        /// @param[in] index
        /// @param[out] capacity 容量, 单位mWh
        /// @return 成功返回true, 失败返回false
        bool GetBatteryFullChargedCapacity(IN int index, OUT unsigned long& capacity);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LBatteryFullCapacityManager(const LBatteryFullCapacityManager&);
        LBatteryFullCapacityManager& operator = (const LBatteryFullCapacityManager&);
    };

    /// @brief 物理网卡管理者类
    class LNetworkAdapterManager
    {
    public:
        LNetworkAdapterManager();
        ~LNetworkAdapterManager();

        /// @brief  获取网卡数量
        /// @return 网卡数量
        int GetNetworkCardCount();

        /// @brief 获取网卡名称
        /// @param[in] index 网卡索引
        /// @param[out] name 存储网卡名称
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardName(IN int index, OUT wstring& name);

        /// @brief 获取网卡制造商
        /// @param[in] idex 网卡索引
        /// @param[out] manufacturer 存储网卡制造商
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取网卡MAC地址
        /// @param[in] index 网卡索引
        /// @param[out] mac 存储MAC地址
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardMACAddress(IN int index, OUT wstring& mac);

        /// @brief 获取网卡连接ID
        /// 如Wi-Fi, Ethernet
        /// @param[in] index 网卡索引
        /// @param[out] connectionID 连接ID
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardConnectionID(IN int index, OUT wstring& connectionID);

        /// @brief 获取网卡GUID
        /// @param[in] index 网卡索引
        /// @param[out] guid 存储网卡GUID
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardGUID(IN int index, OUT wstring& guid);

        /// @brief 获取网卡PNP设备ID
        /// @param[in] index 网卡索引
        /// @param[out] pnpDeviceID
        /// @return 成功返回true, 失败返回false
        bool GetNetworkCardPNPDeviceID(IN int index, OUT wstring& pnpDeviceID);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LNetworkAdapterManager(const LNetworkAdapterManager&);
        LNetworkAdapterManager& operator = (const LNetworkAdapterManager&);
    };

    /// @brief 光驱管理者类
    class LCDRomDriveManager
    {
    public:
        LCDRomDriveManager();
        ~LCDRomDriveManager();

        /// @brief 获取光驱数量
        /// @return 光驱数量
        int GetCDRomDriveCount();

        /// @brief 获取光驱名称
        /// @param[in] index 光驱索引
        /// @param[out] name 存储光驱名称
        /// @return 成功返回true, 失败返回false
        bool GetCDRomDriveName(IN int index, OUT wstring& name);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LCDRomDriveManager(const LCDRomDriveManager&);
        LCDRomDriveManager& operator = (const LCDRomDriveManager&);
    };

}

#endif