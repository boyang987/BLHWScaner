

#ifndef _LTASKSCHEDULER_H_
#define _LTASKSCHEDULER_H_

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


class CTaskScheduler;

/// @brief 任务计划类
/// 设置好任务计划各个属性后调用Register方法即可在系统中注册计划任务
class LTaskScheduler
{
public:
    /// @brief 删除一个任务计划
    /// @param[in] pFolderName 任务计划所在的文件夹名称, 如果为NULL, 则表示任务计划在根目录下
    /// @param[in] pTaskName 任务计划名称
    /// @return 成功返回true, 失败返回false
    static bool Delete(IN const wchar_t* pFolderName, const wchar_t* pTaskName);

public:
    /// @brief 构造函数
    LTaskScheduler();

    /// @brief 析构函数
    ~LTaskScheduler();

    /// @brief 设置任务主要属性
    /// 该方法为可选项, 如果设置为最高权限, 则需要管理员权限, 不然注册任务计划会失败
    /// @param[in] bHighestLevel 标志是否为最高权限运行, 默认不是最高权限
    /// @return 成功返回true, 失败返回false
    bool Principal(IN bool bHighestLevel);

    /// @brief 设置触发条件
    /// 该方法为必须设置项
    /// @param[in] trigger, 1(启动时触发), 2(登录时触发), 3(指定时间触发), 没有默认值
    /// @param[in] pTime, trigger设置为1或2时不需要设置该值, trigger设置为3时需要设置该值
    /// 时间格式为: 2017-09-21T11:17:20, T为日期和时间的分隔符
    /// @return 成功返回true, 失败返回false
    bool Trigger(IN unsigned int trigger,const wchar_t* pTime);

    /// @brief 设置是否可在DC模式下启动
    /// 该方法为可选项
    /// @param[in] bStartOnBattery 标志是否可以在DC模式下启动, 默认值为不可以
    /// @return 成功返回true, 失败返回false
    bool SetStartOnBattery(IN bool bStartOnBattery);

    /// @brief 设置是否唤醒计算器去执行任务
    /// 该方法为可选项
    /// @param[in] bWakeToRun 标志是否唤醒计算器去执行任务, 默认值为不唤醒
    /// @return 成功返回true, 失败返回false
    bool SetWakeToRun(IN bool bWakeToRun);

    /// @brief 设置任务计划动作
    /// 该方法为必须设置项
    /// @param[in] pExePath 程序路径, 不可以为NULL
    /// @param[in] pParam 程序启动参数, 可以为NULL
    /// @param[in] pWorkingDir 程序工作目录, 可以为NULL, 为NULL表示使用系统目录作为工作目录, 如:"C:\\Windows\\System32"
    /// @return 成功返回true, 失败返回false
    bool Action(IN const wchar_t* pExePath, IN const wchar_t* pParam, IN const wchar_t* pWorkingDir);

    /// @brief 将任务计划注册到系统中
    /// 该方法为必须设置项
    /// 如果系统中已经存在同名的计划任务, 则会使用新的属性来覆盖
    /// @param[in] pFolderName 文件夹名称, 如果不存在该文件夹, 则创建文件夹, 如: "Test\\MyFolder", 如果该值为NULL, 则在根目录下创建任务计划
    /// @param[in] pTaskName 任务计划名称, 不可以为NULL
    /// @return 成功返回true, 失败返回false
    bool Register(IN const wchar_t* pFolderName, const wchar_t* pTaskName);

private:
    CTaskScheduler* m_pTaskScheduler; ///< 任务计划实现对象
};

#endif