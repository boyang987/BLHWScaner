/// @file LDiskSpeed.h
/// @brief 该头文件中声明了磁盘速度测试类 
/// 
/// Detail:
/// @author Burnell Liu Email:burnell_liu@outlook.com
/// @version   
/// @date 2016:1:13

#ifndef _LDISKSPEED_H_
#define _LDISKSPEED_H_

#include <string>
using std::wstring;


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

class CDiskSequenceTest;
class CDisk4KRandomTest;

/// @brief 磁盘速度测试错误码
enum LDiskSpeedTestError
{
    DST_NO_ERROR = 0, ///< 没有错误
    DST_OPEN_FILE_ERROR, ///< 打开文件错误
    DST_WRITE_FILE_ERROR, ///< 写文件错误
    DST_READ_FILE_ERROR, ///< 读文件错误
    DST_ALLOC_MEMORY_ERROR, ///< 分配内存错误
    DST_TEST_ABORT_ERROR ///< 测试中断错误
};

/// @brief 磁盘速度测试状态
struct LDiskSpeedTestState 
{
    bool TestDone; ///< 标记测试是否已经结束
    float ReadSpeed; ///< 读速度, 单位M/S
    float WriteSpeed; ///< 写速度, 单位M/S
    LDiskSpeedTestError Error; ///< 错误码
    wstring ErrorMsg; ///< 简略错误消息
    wstring ErrorMsgWindows; ///< Windows错误消息
};

/// @brief 磁盘速度测试接口
class IDiskSpeedTest
{
public:
    /// @brief 析构函数
    virtual ~IDiskSpeedTest() = 0 {};

    /// @brief 开始测试
    /// 该方法为异步方法, 开始测试后使用GetState可以获得测试结果
    /// 注意: 针对系统盘目录需要管理员权限
    /// @param[in] filePath 测试文件完整路径
    /// @param[in] fileSize 测试文件大小, 单位为M
    /// @return 成功返回true, 失败返回false, 测试正在进行则会失败
    virtual bool Start(IN const wstring& filePath, IN unsigned int fileSize) = 0;

    /// @brief 获取测试状态
    /// @return 测试状态
    virtual LDiskSpeedTestState GetState() = 0;

    /// @brief 停止测试
    virtual void Stop() = 0;
};



/// @brief 磁盘顺序测试
/// 磁盘顺序测速要求文件大小大于等于16M, 小于4G
class LDiskSequenceTest : public IDiskSpeedTest
{
public:
    /// @brief 构造函数
    LDiskSequenceTest();

    /// @brief 析构函数
    ~LDiskSequenceTest();

    /// @brief 开始测试
    bool Start(IN const wstring& filePath, IN unsigned int fileSize);

    /// @brief 获取测试状态
    LDiskSpeedTestState GetState();

    /// @brief 停止测试
    void Stop();

private:
    CDiskSequenceTest* m_pDiskSequenceTest; ///< 顺序测试实现对象
};



/// @brief 磁盘4KB随机测试
/// 磁盘顺序测速要求文件小于等于4G
class LDisk4KRandomTest : public IDiskSpeedTest
{
public:
    /// @brief 构造函数
    LDisk4KRandomTest();

    /// @brief 析构函数
    ~LDisk4KRandomTest();

    /// @brief 开始测试
    bool Start(IN const wstring& filePath, IN unsigned int fileSize);

    /// @brief 获取测试状态
    LDiskSpeedTestState GetState();

    /// @brief 停止测试
    void Stop();

private:
    CDisk4KRandomTest* m_pDisk4KRandomTest; ///< 4K随机测试实现对象
};

#endif