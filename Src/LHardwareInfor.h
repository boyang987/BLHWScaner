

#ifndef _LHARDWAREINFOR_H_
#define _LHARDWAREINFOR_H_

#include <string>
using std::string;
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

/// @brief 获取计算机类型
enum COMPUTER_TYPE
{
    COMPUTER_TYPE_UNKNOWN = 0, ///< 未知
    COMPUTER_TYPE_DESKTOP = 1, ///< 台式机
    COMPUTER_TYPE_NOTE_BOOK = 2, ///< 笔记本
    COMPUTER_TYPE_TABLET = 3 ///< 平板电脑
};

/// @brief 计算机系统信息结构
struct ComputerSystemInfor
{
    wstring ModelName; ////< 计算机机种名
    wstring Manufacturer; ///< 计算机制造商
    string Version; ///< 计算机版本
    string SerialNumber; ///< 计算机序列号
    unsigned char UUID[16]; ///< 计算机唯一标识符
    COMPUTER_TYPE Type; ///< 计算机类型
};

/// @brief 操作系统信息结构
struct OperatingSystemInfor
{
    wstring Caption; ////< 标题
    wstring Architecture; ////< 架构
    wstring Version; ////< 版本
    wstring SystemDrive; ////< 系统盘
};

/// @brief 获取BIOS模式
enum BIOS_MODE
{
    BIOS_MODE_UNKNOWN = 0, ///< 未知模式
    BIOS_MODE_LEGACY = 1, ///< 传统模式
    BIOS_MODE_UEFI = 2 ///< UEFI模式
};

/// @brief 主板信息结构
struct MotherBoardInfor
{
    string ProductName; ///< 主板产品名
    string Manufacturer; ///< 主板制造商
    string Version; ///< 主板版本
    string SerialNumber; ///< 主板序列号

    string BiosVendor; ///< Bios厂商
    string BiosVersion; ///< Bios版本
    string BiosReleaseDate; ///< Bios发布日期
    BIOS_MODE BiosMode; ///< Bios模式
    unsigned long BiosRomSize; ///< Bios Rom大小, 单位K
};

/// @brief 处理器信息
struct ProcessorInfor
{
    wstring Name; ///< 名称
    wstring Description; ///< 描述
    wstring Manufacturer; ///< 制造商
    unsigned long CoresNumber; ///< 核心数
    unsigned long LogicalProcessorNumber; ///< 逻辑处理器数量
    unsigned long MaxClockSpeed; ///< 最大频率(MHz)
};

/// @brief 显卡类型
enum VIDEO_CARD_TYPE
{
    VIDEO_CARD_UNKNOWN = 0, ///< 未知
    VIDEO_CARD_INTERNAL, ///< 集成显卡
    VIDEO_CARD_EXTERNAL ///< 独立显卡
};

/// @brief 最大显卡数量
#define MAX_VIDEOCARD_NUMBER 4

/// @brief 显卡信息组
struct VideoCardInforArray
{
    unsigned long Count; ///< 显卡数量
    wstring Description[MAX_VIDEOCARD_NUMBER]; ///< 显卡描述
    wstring DevicePath[MAX_VIDEOCARD_NUMBER]; ///< 显卡设备路径
    VIDEO_CARD_TYPE Type[MAX_VIDEOCARD_NUMBER]; ///< 显卡类型
    unsigned long RAMSize[MAX_VIDEOCARD_NUMBER]; ///< 显存大小, 单位(M)
};

/// @brief 最大物理内存数量
#define MAX_PHYSICAL_MEMORY_NUMBER 8

/// @brief 内存信息组
struct PhysicalMemoryInforArray
{
    unsigned long Count; ///< 内存数量
    wstring Manufacturer[MAX_PHYSICAL_MEMORY_NUMBER]; ///< 制造商
    wstring SerialNumber[MAX_PHYSICAL_MEMORY_NUMBER]; ///< 序列号
    wstring PartNumbe[MAX_PHYSICAL_MEMORY_NUMBER]; ///< 型号
    unsigned long Capacity[MAX_PHYSICAL_MEMORY_NUMBER]; ///< 内存容量, 单位(M)
    unsigned long Speed[MAX_PHYSICAL_MEMORY_NUMBER]; ///< 内存频率
};

/// @brief 最大磁盘驱动器数量
#define MAX_DISK_NUMBER 8

/// @brief 磁盘类型
enum DISK_TYPE
{
    UNKNOWN_DISK = 0, // 未知类型
    FIXED_DISK = 1, // 固定磁盘(如本地硬盘)
    EXTERNAL_USB_DISK = 2, // 扩展磁盘(如USB移动硬盘)
    VIRTUAL_DISK = 3, // 虚拟硬盘(如VHD)
    REMOVABLE_DISK = 4, // 可移动盘
};

/// @brief 固定磁盘类型
enum FIXED_DISK_TYPE
{
    FIXED_DISK_UNKNOWN = 0, // 未知类型
    FIXED_DISK_HDD = 1, // 机械硬盘
    FIXED_DISK_SATA_SSD = 2, // SATA固态硬盘
    FIXED_DISK_EMMC = 3, // EMMC硬盘
    FIXED_DISK_RAID = 4, // 硬盘阵列
    FIXED_DISK_NVME_SSD = 5 // NVME固态硬盘
};

/// @brief 磁盘信息组
struct DiskInforArray 
{
    unsigned long Count; ///< 磁盘数量
    wstring Model[MAX_DISK_NUMBER]; ///< 磁盘型号
    wstring SerialNumber[MAX_DISK_NUMBER]; ///< 磁盘序列号
    unsigned long TotalSize[MAX_DISK_NUMBER]; ///< 磁盘总大小, 单位(G)
    wstring InterfaceType[MAX_DISK_NUMBER]; ///< 磁盘接口类型(SCSI, HDC, IDE, USB 1394)
    DISK_TYPE DiskType[MAX_DISK_NUMBER]; ///< 磁盘类型
    wstring LogicalName[MAX_DISK_NUMBER]; ///< 磁盘逻辑分区名称, 多个分区以分号隔开, 如C:;D:;E:

    bool IsATA[MAX_DISK_NUMBER]; ///< 标识是否为ATA接口硬盘
    struct  
    {
        unsigned long RotationRate; ///< 磁盘转速, 如果值为1表示为固态硬盘(SSD), 0表示获取失败
        unsigned long SATAType; ///< SATA接口类型, 1(SATA1.0 1.5Gb/s), 2(SATA2.0 3.0Gb/s), 3(SATA3.0 6.0Gb/s), 0(获取失败)
        unsigned long PowerOnHours; ///< 通电总时间, 单位hours(小时)
    }ATAInfor[MAX_DISK_NUMBER]; ///< 只有IDE(ATA)接口的硬盘才应该使用该结构中的值

    FIXED_DISK_TYPE FixedDiskType[MAX_DISK_NUMBER]; ///< 只有属于FIXED_DISK分类的硬盘, 该值才有效
};

/// @brief 最大显示器数量
#define MAX_MONITOR_NUMBER 8

/// @brief 显示器信息组
struct MonitorInforArray
{
    unsigned long Count; ///< 显示器数量
    wstring Name[MAX_MONITOR_NUMBER]; ///< 显示器名称
    wstring Date[MAX_MONITOR_NUMBER]; ////< 显示器生产日期
};

/// @brief 电池静态信息
struct BatteryStaticInfor
{
    bool Exist; ///< 标识是否存在电池
    wstring Name; ///< 电池名
    wstring Manufacturer; ///< 电池制造商
    wstring SerialNumber; ///< 电池序列号

    unsigned long DesignedCapacity; ///< 设计容量, 单位mWh
    unsigned long FullChargedCapacity; ///< 充满电容量. 单位mWh
    unsigned long DesignedVoltage; ///< 设计电压, 单位mV
};

/// @brief 最大网卡数量
#define MAX_NETWORKCARD_NUMBER 16

/// @brief 网卡类型
enum NETWORKCARD_TYPE
{
    UNKNOWN_NETCARD = 0, ///< 未知网卡
    WIFI_NETCARD = 1, ///< 无线网卡
    ETHERNET_NETCARD = 2, ///< 网卡
    BLUETOOTH_NETCARD = 3 ///< 蓝牙卡 
};

/// @brief 网卡信息组
struct NetworkCardInforArray
{
    unsigned long Count; ///< 网卡数量
    NETWORKCARD_TYPE Type[MAX_NETWORKCARD_NUMBER]; ///< 网卡类型
    wstring Name[MAX_NETWORKCARD_NUMBER]; ///< 网卡名称
    wstring Manufacturer[MAX_NETWORKCARD_NUMBER]; ///< 网卡制造商
    wstring MACAddress[MAX_NETWORKCARD_NUMBER]; ///< MAC地址
};

/// @brief 最大网卡数量
#define MAX_CDROMDRIVE_NUMBER 8

/// @brief 光驱信息组
struct CDRomDriveInforArray
{
    unsigned long Count; ///< 光驱数量
    wstring Name[MAX_CDROMDRIVE_NUMBER]; ///< 光驱名称
};

/// @brief 最大摄像机数量
#define MAX_CAMERA_NUMBER 8

/// @brief 摄像机信息组
struct CameraInforArray
{
    unsigned long Count; ///< 摄像机数量
    wstring Name[MAX_CAMERA_NUMBER]; ///< 摄像机名称
};

/// @brief 系统度量信息
struct SystemMetricsInfor
{
    bool TouchScreenSupported; ///< 是否支持触摸屏
};

/// @brief 最大生物识别设备数量
#define MAX_BIOMETRIC_NUMBER 8

/// @brief 生物识别设备信息组
struct BiometricInforArray
{
    unsigned long Count; ///< 生物识别设备数量
    wstring Description[MAX_BIOMETRIC_NUMBER]; ///< 设备描述
    wstring Manufacturer[MAX_BIOMETRIC_NUMBER]; ///< 设置制造商
};

/// @brief 硬件信息类
///
/// 该类获取的都是固定信息
class LHardwareInfor
{
public:
    /// @brief 构造函数
    LHardwareInfor();

    /// @brief 析构函数
    ~LHardwareInfor();

    /// @brief 获取计算机系统信息
    /// @return 计算机系统信息
    static const ComputerSystemInfor& GetComputerSystemInfor(); 

    /// @brief 获取操作系统信息
    /// @return 操作系统信息
    static const OperatingSystemInfor& GetOperatingSystemInfor();

    /// @brief 获取主板信息
    /// @return 主板信息
    static const MotherBoardInfor& GetMotherBoardInfor();

    /// @brief 获取处理器信息
    /// @return 处理器信息
    static const ProcessorInfor& GetProcessorInfor();

    /// @brief 获取显卡信息
    /// @return 显卡信息
    static const VideoCardInforArray& GetVideoCardInfor();

    /// @brief 获取物理内存信息
    /// @return 物理内存信息
    static const PhysicalMemoryInforArray& GetPhysicalMemoryInfor();

    /// @brief 获取磁盘信息
    /// @return 磁盘信息
    static const DiskInforArray& GetDiskInfor();

    /// @brief 获取显示器信息
    /// @return 显示器信息
    static const MonitorInforArray& GetMonitorInfor();

    /// @brief 获取电池静态信息
    /// @return 电池静态信息
    static const BatteryStaticInfor& GetBatteryStaticInfor();

    /// @brief 获取网卡信息
    /// @return 网卡信息
    static const NetworkCardInforArray& GetNetworkCardInfor();

    /// @brief 获取光驱信息
    /// @return 光驱信息
    static const CDRomDriveInforArray& GetCDRomDriveInfor();

    /// @brief 获取摄像机信息
    /// @return 摄像机信息
    static const CameraInforArray& GetCameraInfor();

    /// @brief 获取系统度量信息
    /// @return 系统度量信息
    static const SystemMetricsInfor& GetSystemMetricsInfor();

    /// @brief 获取生物识别设备信息
    /// @return 生物识别设备信息
    static const BiometricInforArray& GetBiometricInfor();
};

#endif