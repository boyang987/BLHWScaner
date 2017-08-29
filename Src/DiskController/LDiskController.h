

#ifndef _LDISKCONTROLLER_H_
#define _LDISKCONTROLLER_H_


#include <string>
using std::wstring;


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

/// @brief SMART数据长度
#ifndef SMART_DATA_LENGTH
#define SMART_DATA_LENGTH 362
#endif

/// @brief 获取逻辑分区剩余空间
/// @param[in] logicalDrive 逻辑分区名称
/// @param[out] pFreeSpace 存储剩余空间, 单位字节
/// @return 成功返回true, 失败返回false 
bool GetLogicalDriveFreeSpace(IN const wstring& logicalDrive, unsigned long long* pFreeSpace);

class CIDEDiskController;

/// @brief IDE(ATA)磁盘控制器
///
/// 使用该类需要管理员权限
class LIDEDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"\\\\.\\DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    LIDEDiskController(IN const wstring& devicePath);
    ~LIDEDiskController();

    /// @brief 判断设备是否存在
    /// @return 存在返回true, 不存在返回false
    bool DeviceExist();

    /// @brief 获取标定转速
    /// @return 标定转速(单位RPM), 发生错误返回0, 返回值为1表示硬盘为SSD
    unsigned long GetRotationRate();

    /// @brief 获取SATA类型
    /// @return SATA接口类型, 1(SATA1.0 1.5Gb/s), 2(SATA2.0 3.0Gb/s), 3(SATA3.0 6.0Gb/s), 0(获取失败)
    unsigned long GetSATAType();

    /// @brief 获取SMART数据
    /// @param[out] smartData[362] 存储362个字节的SMART数据
    /// @return 成功返回true, 失败返回false
    bool GetSMARTData(OUT unsigned char smartData[SMART_DATA_LENGTH]);

private:
    CIDEDiskController* m_pIDEDiskController; ///< IDE磁盘控制器实现对象
};

#endif