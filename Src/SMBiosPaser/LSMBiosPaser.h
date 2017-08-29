

#ifndef _LSMBIOSPASER_H_
#define _LSMBIOSPARSER_H_

#include <vector>
using std::vector;
#include <string>
using std::string;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif



/// @brief SMBios BIOS 信息
struct SMBiosBIOSInfor
{
    string Vendor; /// <厂商名
    string Version; ///< 版本
    string ReleaseDate; ///< 发布日期
    unsigned long RomSize; ///< ROM大小, 单位K
};

/// @brief SMBios 系统信息
struct SMBiosSystemInfor
{
    string Manufacturer; ///< 电脑制造商
    string ProductName; ///< 电脑名称
    string Version; ///< 电脑版本
    string SerialNumber; ///< 电脑序列号
    unsigned char UUID[16]; ///< 电脑唯一标识符
};

/// @brief SMBios 主板信息
struct SMBiosBaseBoardInfor
{
    string Manufacturer; ///< 主板制造商
    string Product; ///< 主板名
    string Version; ///< 主板版本
    string SerialNumber; ///< 主板序列号
};

class CSMBiosPaser;

/// @brief SMBios解析类
class LSMBiosPaser
{
public:
    /// @brief 构造函数
    /// @param[in] smBiosData SMBios数据
    explicit LSMBiosPaser(IN const vector<unsigned char>& smBiosData);

    /// @brief 析构函数
    ~LSMBiosPaser();

    /// @brief 获取BIOS信息
    /// @param[out] biosInfor 存储BIOS信息
    /// @return 成功返回true, 失败返回false
    bool GetBiosInfor(OUT SMBiosBIOSInfor& biosInfor);

    /// @brief 获取系统信息
    /// @param[out] systemInfor 存储系统信息
    /// @return 成功返回true, 失败返回false
    bool GetSystemInfor(OUT SMBiosSystemInfor& systemInfor);

    /// @brief 获取主板信息
    /// @param[out] baseBoardInfor 存储主板信息
    /// @return 成功返回true, 失败返回false
    bool GetBaseBoardInfor(OUT SMBiosBaseBoardInfor& baseBoardInfor);

private:
    CSMBiosPaser* m_pSMBiosPaser; ///< SMBios解析对象
};



#endif