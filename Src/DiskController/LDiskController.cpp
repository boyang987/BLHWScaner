
#include "LDiskController.h"


#include <string>
using std::string;

#include <Windows.h>
#include <ntddscsi.h>


/// @brief 存储设备属性
struct SStorageDeviceProperty
{
    bool RemovableMedia; // 是否为可移动媒体
    UCHAR DeviceType; // 设备类型, 由SCSI规范定义
    UCHAR DeviceTypeModifier; // 设备类型修饰, 由SCSI规范定义, 为0表示没有该值
    STORAGE_BUS_TYPE BusType; // 总线类型
    string VendorId; // 厂商ID
    string ProductId; // 产品ID
    string ProductRevision; // 产品修订
    string SerialNumber; // 序列号
};

// 以下结构取消内存对齐
#pragma pack (1) // 取消内存对齐

/// @brief ATA8 ID结构
///
/// 共512字节(256个WORD)，这里仅定义了一些感兴趣的项(基本上依据ATA8-ACS)
/// 注意:该结构中的字符串字符两两颠倒 原因是ATA/ATAPI中的WORD，与Windows采用的字节顺序相反
struct SATA8IdentifyData
{
    USHORT GeneralConfig; // WORD 0: 基本信息字
    USHORT Obsolete1; // WORD 1: 废弃
    USHORT SpecConfig; // WORD 2: 具体配置
    USHORT Obsolete3; // WORD 3: 废弃
    USHORT Obsolete4; // WORD 4: 废弃
    USHORT Obsolete5; // WORD 5: 废弃
    USHORT Obsolete6; // WORD 6: 废弃
    USHORT CompactFlashReserved[2]; // WORD 7-8: 保留
    USHORT Obsolete9; // WORD 9: 废弃
    CHAR SerialNumber[20]; // WORD 10-19:序列号
    USHORT Obsolete20[3]; // WORD 20-22: 废弃
    CHAR FirmwareRev[8]; // WORD 23-26: 固件版本
    CHAR ModelNumber[40]; // WORD 27-46: 型号
    USHORT Reserved47; // WORD 47: 保留
    USHORT Reserved48; // WORD 48: 保留
    struct 
    {
        USHORT Obsolete0:8; // bit 0-7: 废弃
        USHORT DMASupport:1; // bit 8: 1=支持DMA
        USHORT LBASupport:1; // bit 9: 1=支持LBA
        USHORT IORDYDisabled:1; // bit 10: 1=IORDY可以被禁用
        USHORT IORDYSupport:1; // bit 11: 1=支持IORDY
        USHORT Reserved12:4; // bit 12-15: 保留
    }Capabilities; // WORD 49: 一般能力
    USHORT Reserved50; // WORD 50: 保留
    USHORT Obsolete51; // WORD 51: 废弃
    USHORT Obsolete52; // WORD 52: 废弃
    USHORT Reserved53; // WORD 53: 保留
    USHORT Obsolete54[5]; // WORD 54-58: 废弃
    USHORT Reserved59; // WORD 59: 保留
    ULONG LBATotalSectors; // WORD 60-61: LBA可寻址的扇区数
    USHORT Obsolete62; // WORD 62: 废弃
    struct 
    {
        USHORT Mode0:1; // bit 0: 1=支持模式0 (4.17Mb/s)
        USHORT Mode1:1; // bit 1: 1=支持模式1 (13.3Mb/s)
        USHORT Mode2:1; // bit 2: 1=支持模式2 (16.7Mb/s)
        USHORT Reserved5:5; // bit 3-7: 保留
        USHORT Mode0Sel:1; // bit8: 1=已选择模式0
        USHORT Mode1Sel:1; // bit9: 1=已选择模式1
        USHORT Mode2Sel:1; // bit10: 1=已选择模式2
        USHORT Reserved11:5; // bit 11-15: 保留
    } MultiWordDMA; // WORD 63: 多字节DMA支持能力
    struct 
    {
        USHORT AdvPOIModes:8; // bit 0-7: 支持高级POI模式数
        USHORT Reserved8:8; // bit 8-15: 保留
    } PIOCapacity; // WORD 64: 高级PIO支持能力
    USHORT MinMultiWordDMACycle; // WORD 65: 多字节DMA传输周期的最小值
    USHORT RecMultiWordDMACycle; // WORD 66: 多字节DMA传输周期的建议值
    USHORT MinPIONoFlowCycle; // WORD 67: 无流控制时PIO传输周期的最小值
    USHORT MinPIOFlowCycle; // WORD 68: 有流控制时PIO传输周期的最小值
    USHORT Reserved69[7]; // WORD 69-75: 保留
    struct
    {
        USHORT Reserved0:1; // bit 0: 保留
        USHORT SATAGen1:1; // bit1: 1=支持SATA Gen1(1.5Gb/s)
        USHORT SATAGen2:1; // bit2: 1=支持SATA Gen2(3.0Gb/s)
        USHORT SATAGen3:1; // bit3: 1=支持SATA Gen3(6.0Gb/s)
        USHORT Reserved4:12; // bit4-15: 保留
    }SATACapabilities; // WORD 76: SATA能力
    USHORT Reserved77; // WORD 77: 保留
    struct
    {
        USHORT Reserved0: 1; // bit0: 应该为0
        USHORT NoneZeroBufferOffsets: 1; // bit1: 1=设备支持非0缓冲偏移
        USHORT DMASetupAutoActivation: 1; // bit2:
        USHORT InitiatePowerManagement: 1; // bit3: 1=设备支持发起电源管理
        USHORT InorderDataDelivery: 1; // bit4:
        USHORT Reserved11: 11; // bit5-15: 保留
    }SATAFeaturesSupported; // WORD 78: SATA特征支持
    struct
    {
        USHORT Reserved0: 1; // bit0: 应该为0
        USHORT NoneZeroBufferOffsets: 1; // bit1: 1=非0缓冲偏移开启
        USHORT DMASetupAutoActivation: 1; // bit2:
        USHORT InitiatePowerManagement: 1; // bit3: 1=发起电源管理开启
        USHORT InorderDataDelivery: 1; // bit4:
        USHORT Reserved11: 11; // bit5-15: 保留
    }SATAFeaturesEnabled; // WORD 79: SATA特征开启
    struct 
    {
        USHORT Reserved0:1; // bit0: 保留
        USHORT Obsolete1:3; // bit1-3: 废弃
        USHORT ATA4:1; // bit4: 1=支持ATA/ATAPI-4
        USHORT ATA5:1; // bit5: 1=支持ATA/ATAPI-5
        USHORT ATA6:1; // bit6: 1=支持ATA/ATAPI-6
        USHORT ATA7:1; // bit7: 1=支持ATA/ATAPI-7
        USHORT ATA8:1; // bit8: 1=支持ATA8-ACS
        USHORT Reserved9:7; // bit9-15: 保留
    } MajorVersion; // WORD 80: 主版本
    USHORT MinorVersion; // WORD 81: 副版本
    USHORT Reserved82;// WORD 82: 保留
    struct 
    {
        USHORT Reserved0:3; // bit0-2: 保留
        USHORT AdvancedPowerManagementFeatureSetSupported:1; // bit3: 1=高级电源管理特征集支持
        USHORT Reserved4:12; // bit4-15: 保留
    }CommandSetsSupported; // WORD 83: 命令集支持
    USHORT Reserved84[2]; // WORD 84-85: 保留
    struct 
    {
        USHORT Reserved0:3; // bit0-2: 保留
        USHORT AdvancedPowerManagementFeatureSetEnabled:1; // bit3: 1=高级电源管理特征集开启
        USHORT Reserved4:12; // bit4-15: 保留
    }CommandSetFeatureEnabledSupported;  // WORD 86: 命令集或特征开启或支持
    USHORT Reserved87; // WORD 87: 保留
    struct 
    {
        USHORT Mode0:1;                // 1=支持模式0 (16.7Mb/s)
        USHORT Mode1:1;                // 1=支持模式1 (25Mb/s)
        USHORT Mode2:1;                // 1=支持模式2 (33Mb/s)
        USHORT Mode3:1;                // 1=支持模式3 (44Mb/s)
        USHORT Mode4:1;                // 1=支持模式4 (66Mb/s)
        USHORT Mode5:1;                // 1=支持模式5 (100Mb/s)
        USHORT Mode6:1;                // 1=支持模式6 (133Mb/s)
        USHORT Reserved7:1;          // 保留
        USHORT Mode0Sel:1;             // 1=已选择模式0
        USHORT Mode1Sel:1;             // 1=已选择模式1
        USHORT Mode2Sel:1;             // 1=已选择模式2
        USHORT Mode3Sel:1;             // 1=已选择模式3
        USHORT Mode4Sel:1;             // 1=已选择模式4
        USHORT Mode5Sel:1;             // 1=已选择模式5
        USHORT Mode6Sel:1;             // 1=已选择模式6
        USHORT Reserved15:1;          // 保留
    } UltraDMA; // WORD 88:  Ultra DMA支持能力
    USHORT Reserved89[2];         // WORD 89-90: 保留
    struct
    {
        BYTE LevelValue; // 高级电源管理级别值
        BYTE Reserved;
    }AdvancePowerManagementLevel; // WORD 91: 高级电源管理级别
    USHORT Reserved92[125];         // WORD 92-216
    USHORT NominalMediaRotationRate; //  WORD 217 标定转速(RPM), 如果值为1表示为SSD或者其他
    USHORT Reserved218[38]; // WORD 218-255 保留
};

#pragma pack () // 恢复内存对齐

/// @brief 通用存储控制器
///
///https://msdn.microsoft.com/en-us/library/windows/hardware/ff553891(v=vs.85).aspx
class CGeneralStorageController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: 
    /// 硬盘逻辑分区: C:, D:, E:, ...
    /// 物理驱动器: PhysicalDrive0, PhysicalDrive1, ...
    /// CD-ROM, DVD/ROM: CDROM0, CDROM1, ...
    explicit CGeneralStorageController(IN const wstring& devicePath);
    virtual ~CGeneralStorageController();

    /// @brief 判断设备是否存在
    /// @return 存在返回true, 不存在返回false
    bool DeviceExist();

    /// @brief 重置设备路径
    /// @param[in] devicePath 设备路径
    void ResetDeviceId(IN const wstring& devicePath);

    /// @brief 获取设备属性
    /// @param[out] devicePrperty 存储设备属性
    /// @return 成功返回true, 失败返回false
    bool GetDeviceProperty(OUT SStorageDeviceProperty& devicePrperty);

    /// @brief 获取媒体类型
    /// @param[in] mediaInfo 存储媒体信息
    /// @return 成功返回true, 失败返回false
    bool GetMediaType(OUT DEVICE_MEDIA_INFO& mediaInfo);

protected:
    /// @brief 打开设备句柄
    /// @return 返回设备句柄
    HANDLE OpenDeviceHandle();

private:
    wstring m_devicePath; ///< 设备名称

private:
    CGeneralStorageController(const CGeneralStorageController&);
    CGeneralStorageController& operator = (const CGeneralStorageController&);
};

/// @brief 磁盘控制器
///
///https://msdn.microsoft.com/en-us/library/windows/hardware/ff552626(v=vs.85).aspx
class CDiskController : public CGeneralStorageController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    CDiskController(IN const wstring& devicePath);
    virtual ~CDiskController();

    /// @brief 获取磁盘几何信息
    /// @param[in] geometry 存储几何信息
    /// @return 成功返回true, 失败返回false
    bool GetGeometry(OUT DISK_GEOMETRY& geometry);

    /// @brief 获取版本信息
    ///
    /// 该方法要求磁盘支持SMART
    /// @param[in] versionParams 存储几何信息
    /// @return 成功返回true, 失败返回false
    bool GetVersionInfor(OUT GETVERSIONINPARAMS& versionParams);
};

/// @brief IDE(ATA)磁盘控制器
///
/// https://msdn.microsoft.com/en-us/library/windows/hardware/ff559105(v=vs.85).aspx
/// ATA8-ACS
class CIDEDiskController : public CDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    CIDEDiskController(IN const wstring& devicePath);
    virtual ~CIDEDiskController();

    bool ATACmdCheckMediaCardType();

    /// @brief 检测电源模式
    /// @param[out] mode 存储模式的值
    /// mode 的值如下: (查阅ATA8 SPE)
    /// 0x00 Device is in Standby mode [省电模式]
    /// 0x40 Device is in NV Cache Power Mode and spindle is spun down or spinning down
    /// 0x41 Device is in NV Cache Power Mode and spindle is spun up or spinning up
    /// 0x80 Device is in Idle mode
    /// 0xFF Device is in Active mode or Idle mode
    /// @return 成功返回true, 失败返回false
    bool ATACmdCheckPowerMode(OUT BYTE& mode);

    /// @brief 直接设置电源模式为IDLE
    /// @return 成功返回true, 失败返回false
    bool ATACmdIDLEImmediate();

    /// @brief 直接设置电源模式为Standby
    /// @return 成功返回true, 失败返回false
    bool ATACmdStandbyImmediate();

    /// @brief 睡眠命令
    /// @return 成功返回true, 失败返回false
    bool ATACmdSleep();

    /// @brief 重置命令
    /// @return 成功返回true, 失败返回false
    bool ATACmdDeviceReset();

    /// @brief 诊断命令
    /// @param[out] state 存储状态值
    /// state 的值为0x01或0x81则表示设备是好的, 否则表示设备是坏的或者没有连接
    /// @return 成功返回true, 失败返回false
    bool ATACmdExecuteDeviceDiagnostic(OUT BYTE& state);

    /// @brief 检验读扇区扩展命令(没有返回读取的数据, For LBA48)
    /// @param[in] lbaAddress 需要读取的起始扇区开始地址
    /// @param[in] sectorCount 需要读取的扇区的数量
    /// @return 成功返回true, 失败返回false
    bool ATACmdReadVerifySectorExt(IN ULONGLONG lbaAddress, IN unsigned long sectorCount);

    /// @brief 获取ID信息命令
    /// @param[out] identifyData 存储ID信息
    /// @return 成功返回true, 失败返回false
    bool ATACmdIdentifyDevice(OUT SATA8IdentifyData& identifyData);

    /// @brief 获取SMART数据命令
    ///
    /// SMART属性定义: http://en.wikipedia.org/wiki/S.M.A.R.T.
    /// @param[out] smartData 存储SMART数据
    /// @return 成功返回true, 失败返回false
    bool ATACmdSMARTReadData(OUT unsigned char smartData[SMART_DATA_LENGTH]);
private:
    /// @brief ATA命令
    enum ATA_COMMAND
    {
        ATA_DEVICE_RESET = 0x08, // 重置命令
        ATA_READ_VERIFY_SECTOR_EXT = 0x42, // 检验读扩展命令
        ATA_EXECUTE_DEVICE_DIAGNOSTIC = 0x90, // 诊断命令
        ATA_CHECK_MEDIA_CARD_TYPE = 0xD1, // 检测媒体类型命令
        ATA_STANDBY_IMMEDIATE = 0xE0, // 立即设置电源模式为STANDBY命令
        ATA_IDELE_IMMEDIATE = 0xE1, // 立即设置电源模式为IDELE命令
        ATA_IDENTIFY_DEVICE = 0xEC, // 获取ID信息命令
        ATA_CHECK_POWER_MODE = 0xE5, // 检测电源模式命令
        ATA_SLEEP = 0xE6, // 随眠命令
        ATA_SET_FEATURE = 0xFE // 设置特征命令
    };

    /// @brief ATA命令设置特征子命令
    enum SET_FEATURE_SUB_COMMAND
    {
        SET_FEATURE_ENABLE_ADVANCED_POWER_MANAGEMENT = 0x05, // 开启高级电源管理
        SET_FEATURE_DISABLE_ADVANCED_POWER_MANAGEMENT = 0x85 // 关闭高级电源管理
    };
};

CGeneralStorageController::CGeneralStorageController(IN const wstring& devicePath)
    : m_devicePath(devicePath)
{
}

CGeneralStorageController::~CGeneralStorageController()
{

}

bool CGeneralStorageController::DeviceExist()
{
    HANDLE hDevice =  this->OpenDeviceHandle();

    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        return false;
    }

    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;

        return true;
    }

    return true;
}

void CGeneralStorageController::ResetDeviceId(IN const wstring& devicePath)
{
    m_devicePath = devicePath;
}

bool CGeneralStorageController::GetDeviceProperty(OUT SStorageDeviceProperty& devicePrperty)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes; // 存储返回的字节数
    STORAGE_PROPERTY_QUERY propertyQuery; // 查询的属性
    STORAGE_DEVICE_DESCRIPTOR deviceDescriptor; // 属性描述结构
    STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = NULL; // 存储查询到的属性

    // 打开设备
    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 填充需要查询的属性
    ZeroMemory(&propertyQuery, sizeof(propertyQuery));
    propertyQuery.PropertyId = StorageDeviceProperty;
    propertyQuery.QueryType = PropertyStandardQuery;

    // 第一次查询, 获取属性需要的字节数
    nRet = DeviceIoControl(
        hDevice, 
        IOCTL_STORAGE_QUERY_PROPERTY, 
        &propertyQuery, 
        sizeof(propertyQuery), 
        &deviceDescriptor, 
        sizeof(deviceDescriptor), 
        &nBytes, 
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)malloc(deviceDescriptor.Size);

    // 第二次查询, 获取属性
    nRet = DeviceIoControl(
        hDevice, 
        IOCTL_STORAGE_QUERY_PROPERTY, 
        &propertyQuery, 
        sizeof(propertyQuery), 
        pDeviceDescriptor, 
        deviceDescriptor.Size, 
        &nBytes, 
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

    devicePrperty.BusType = pDeviceDescriptor->BusType;
    devicePrperty.DeviceType = pDeviceDescriptor->DeviceType;
    devicePrperty.DeviceTypeModifier = pDeviceDescriptor->DeviceTypeModifier;
    devicePrperty.RemovableMedia = pDeviceDescriptor->RemovableMedia == TRUE ? true : false;

    // 序列号以及其他属性需要根据偏移来取得, 如果偏移值为0, 则表示没有该值
    if (pDeviceDescriptor->SerialNumberOffset != 0)
    {
        devicePrperty.SerialNumber = (char*)pDeviceDescriptor + pDeviceDescriptor->SerialNumberOffset;
    }
    if (pDeviceDescriptor->VendorIdOffset != 0)
    {
        devicePrperty.VendorId = (char*)pDeviceDescriptor + pDeviceDescriptor->VendorIdOffset;
    }
    if (pDeviceDescriptor->ProductIdOffset != 0)
    {
        devicePrperty.ProductId = (char*)pDeviceDescriptor + pDeviceDescriptor->ProductIdOffset;
    }
    if (pDeviceDescriptor->ProductRevisionOffset != 0)
    {
        devicePrperty.ProductRevision = (char*)pDeviceDescriptor + pDeviceDescriptor->ProductRevisionOffset;
    }

SAFE_EXIT:

    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    if (pDeviceDescriptor != NULL)
    {
        free(pDeviceDescriptor);
        pDeviceDescriptor = NULL;
    }

    return bRet;
}

bool CGeneralStorageController::GetMediaType(OUT DEVICE_MEDIA_INFO& mediaInfo)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数
    ULONG requiredBytes = 0; // 存储需要的字节数

    GET_MEDIA_TYPES mediaTypes; // 媒体类型结构
    GET_MEDIA_TYPES* pMediaTypes = NULL; // 存储媒体类型

    // 打开设备
    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 第一次查询需要的大小
    nRet = DeviceIoControl(
        hDevice, 
        IOCTL_STORAGE_GET_MEDIA_TYPES_EX, 
        NULL, 
        0, 
        &mediaTypes, 
        sizeof(mediaTypes), 
        &nBytes, 
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    requiredBytes = sizeof(GET_MEDIA_TYPES) + mediaTypes.MediaInfoCount * sizeof(DEVICE_MEDIA_INFO);
    // 分配足够的空间
    pMediaTypes = (GET_MEDIA_TYPES*)malloc(requiredBytes);

    // 第二次查询, 获取类型
    nRet = DeviceIoControl(
        hDevice, 
        IOCTL_STORAGE_GET_MEDIA_TYPES_EX, 
        NULL, 
        0, 
        pMediaTypes, 
        requiredBytes, 
        &nBytes, 
        NULL);

    // 虽然可能获取到多组媒体信息, 但我们只取第一组, 以后如果有其他需求, 
    // 你可以重载本方法
    mediaInfo = *(pMediaTypes->MediaInfo);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:

    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    if (pMediaTypes != NULL)
    {
        free(pMediaTypes);
        pMediaTypes = NULL;
    }

    return bRet;
}

HANDLE CGeneralStorageController::OpenDeviceHandle()
{
    HANDLE hDevice =  CreateFileW(
		m_devicePath.c_str(), 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, 
		0, 
		NULL);
    return hDevice;
}

CDiskController::CDiskController(IN const wstring& devicePath)
    : CGeneralStorageController(devicePath)
{

}

CDiskController::~CDiskController()
{

}

bool CDiskController::GetGeometry(OUT DISK_GEOMETRY& geometry)
{
    HANDLE hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        return false;

    DWORD nBytes = 0;
    BOOL nRet = DeviceIoControl(
        hDevice, 
        IOCTL_DISK_GET_DRIVE_GEOMETRY, 
        NULL, 
        0, 
        &geometry, 
        sizeof(geometry), 
        &nBytes, 
        NULL);

    bool bRet = false;
    if (nRet == FALSE)
    {
        bRet = false;
    }
    else
    {
        bRet = true;
    }


    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CDiskController::GetVersionInfor(OUT GETVERSIONINPARAMS& versionParams)
{
    HANDLE hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        return false;

    DWORD nBytes = 0;
    BOOL nRet = DeviceIoControl(
        hDevice, 
        SMART_GET_VERSION, 
        NULL, 
        0, 
        &versionParams, 
        sizeof(versionParams), 
        &nBytes, 
        NULL);

    bool bRet = false;
    if (nRet == FALSE)
    {
        bRet = false;
    }
    else
    {
        bRet = true;
    }


    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

CIDEDiskController::CIDEDiskController(IN const wstring& devicePath)
    : CDiskController(devicePath)
{

}

CIDEDiskController::~CIDEDiskController()
{

}

bool CIDEDiskController::ATACmdCheckMediaCardType()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    
    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_CHECK_MEDIA_CARD_TYPE; // 命令寄存器
   
    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }
   
    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdCheckPowerMode(OUT BYTE& mode)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_CHECK_POWER_MODE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 查阅ATA8 SPEC 可知, 执行命令后TaskFile中的第2个字节值为电源模式的值
    mode = pATACmd->CurrentTaskFile[1];

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdIDLEImmediate()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_IDELE_IMMEDIATE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdStandbyImmediate()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_STANDBY_IMMEDIATE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdSleep()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_SLEEP; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdDeviceReset()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_DEVICE_RESET; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdExecuteDeviceDiagnostic(OUT BYTE& state)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_EXECUTE_DEVICE_DIAGNOSTIC; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 获取错误寄存器中的值
    // 执行命令后TaskFile中的第1个字节值为错误寄存器的值
    state = pATACmd->CurrentTaskFile[0];
    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}





bool CIDEDiskController::ATACmdReadVerifySectorExt(IN ULONGLONG lbaAddress, IN unsigned long sectorCount)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数
    IDEREGS* pCurrentTaskFile = (IDEREGS*)pATACmd->CurrentTaskFile;
    IDEREGS* pPreviousTaskFile = (IDEREGS*)pATACmd->PreviousTaskFile;

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_48BIT_COMMAND; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)

    // 设置命令寄存器
    pCurrentTaskFile->bCommandReg = pPreviousTaskFile->bCommandReg = ATA_READ_VERIFY_SECTOR_EXT;

     /*	
    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |           |   \_____________________/
    |           |              |
    |           |              `------------ If L=0, Head Select.
    |           |                                   These four bits select the head number.
    |           |                                   HS0 is the least significant.
    |           |                            If L=1, HS0 through HS3 contain bit 24-27 of the LBA.
    |           `--------------------------- Drive. When DRV=0, drive 0 (master) is selected. 
    |                                               When DRV=1, drive 1 (slave) is selected.
    `--------------------------------------- LBA mode. This bit selects the mode of operation.
    When L=0, addressing is by 'CHS' mode.
    When L=1, addressing is by 'LBA' mode.
    */

    // 设置驱动器头寄存器
    pCurrentTaskFile->bDriveHeadReg = pPreviousTaskFile->bDriveHeadReg = 0xE0; // 驱动器头寄存器设置为0xE0表示使用LBA寻址方式
   

    // 设置读取扇区数目寄存器
    pCurrentTaskFile->bSectorCountReg = (BYTE)sectorCount;
    pPreviousTaskFile->bSectorCountReg = (BYTE)(sectorCount >> 8);

    // 设置起始寄存器LBA地址
    pCurrentTaskFile->bSectorNumberReg = (BYTE)lbaAddress;
    pCurrentTaskFile->bCylLowReg = (BYTE)(lbaAddress >> 8);
    pCurrentTaskFile->bCylHighReg = (BYTE)(lbaAddress >> 16);
    pPreviousTaskFile->bSectorNumberReg = (BYTE)(lbaAddress >> 24);
    pPreviousTaskFile->bCylLowReg = (BYTE)(lbaAddress >> 32);
    pPreviousTaskFile->bCylHighReg = (BYTE)(lbaAddress >> 40);

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdIdentifyDevice(OUT SATA8IdentifyData& identifyData)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_IDENTIFY_DEVICE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    memcpy(&identifyData, paramBuffer + sizeof(ATA_PASS_THROUGH_EX), DATA_BUFFER_LEN);

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdSMARTReadData(OUT unsigned char smartData[SMART_DATA_LENGTH])
{
     BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数
    IDEREGS* pCurrentTaskFile = (IDEREGS*)pATACmd->CurrentTaskFile;

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)

    // 设置命令寄存器
    pCurrentTaskFile->bCommandReg = SMART_CMD;

     /*	
    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |           |   \_____________________/
    |           |              |
    |           |              `------------ If L=0, Head Select.
    |           |                                   These four bits select the head number.
    |           |                                   HS0 is the least significant.
    |           |                            If L=1, HS0 through HS3 contain bit 24-27 of the LBA.
    |           `--------------------------- Drive. When DRV=0, drive 0 (master) is selected. 
    |                                               When DRV=1, drive 1 (slave) is selected.
    `--------------------------------------- LBA mode. This bit selects the mode of operation.
    When L=0, addressing is by 'CHS' mode.
    When L=1, addressing is by 'LBA' mode.
    */

    // 设置驱动器头寄存器
    pCurrentTaskFile->bDriveHeadReg = 0xA0; // 驱动器头寄存器设置为0xA0表示使用CHS寻址方式
   

    // 设置特征寄存器
    pCurrentTaskFile->bFeaturesReg = READ_ATTRIBUTES;

    pCurrentTaskFile->bCylLowReg = SMART_CYL_LOW;
    pCurrentTaskFile->bCylHighReg = SMART_CYL_HI;

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 返回的数据中, 前362个数据为SMART属性数据
    memcpy(smartData, paramBuffer + sizeof(ATA_PASS_THROUGH_EX), SMART_DATA_LENGTH);

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool GetLogicalDriveFreeSpace(IN const wstring& logicalDrive, unsigned long long* pFreeSpace)
{
    if (pFreeSpace == 0)
        return false;

    if (logicalDrive.empty())
        return false;

    (*pFreeSpace) = 0;

    ULARGE_INTEGER freeSpace;
    BOOL iRet = GetDiskFreeSpaceExW(logicalDrive.c_str(), &freeSpace, 0, 0);
    if (iRet == FALSE)
        return false;

    (*pFreeSpace) = freeSpace.QuadPart;


    return true;
}

LIDEDiskController::LIDEDiskController(IN const wstring& devicePath)
{
    m_pIDEDiskController = new CIDEDiskController(devicePath);
}

LIDEDiskController::~LIDEDiskController()
{
    delete m_pIDEDiskController;
    m_pIDEDiskController = 0;
}

bool LIDEDiskController::DeviceExist()
{
    return m_pIDEDiskController->DeviceExist();
}

unsigned long LIDEDiskController::GetRotationRate()
{
    SATA8IdentifyData identifyData;
    bool bRet = this->m_pIDEDiskController->ATACmdIdentifyDevice(identifyData);
    if (!bRet)
    {
        return 0;
    }

    return identifyData.NominalMediaRotationRate;
}

unsigned long LIDEDiskController::GetSATAType()
{
    SATA8IdentifyData identifyData;
    bool bRet = this->m_pIDEDiskController->ATACmdIdentifyDevice(identifyData);
    if (!bRet)
    {
        return 0;
    }

    /*
    高版本的包含低版本的值所以需要从高到低判断
    */
    if (identifyData.SATACapabilities.SATAGen3)
    {
        return 3;
    }

    if (identifyData.SATACapabilities.SATAGen2)
    {
        return 2;
    }

    if (identifyData.SATACapabilities.SATAGen1)
    {
        return 1;
    }

    return 0;
}

bool LIDEDiskController::GetSMARTData(OUT unsigned char smartData[SMART_DATA_LENGTH])
{
    return m_pIDEDiskController->ATACmdSMARTReadData(smartData);
}