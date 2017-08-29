

#ifndef _LLOG_H_
#define _LLOG_H_

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

#include <string>
using std::string;
using std::wstring;

/// @brief LOG类
namespace LLog
{
    /// @brief 打开LOG档, 如果不存在则创建, 如果已经存在那么原始内容将被销毁
    /// @param[in] szFileName LOG档文件名
    /// @return 成功返回true, 失败返回false
    bool Open(IN const wchar_t* szFileName);

    /// @brief 关闭LOG档
    void Close();

    /// @brief 每行LOG前显示线程Id
    /// 默认不增加线程Id
    /// @param[in] bFlag 标识是否在每行LOG前显示线程Id
    void ShowThreadId(IN bool bFlag);

    /// @brief 每行LOG前显示当前时间
    /// 默认不增加当前时间
    /// @param[in] bFlag 标识是否在每行LOG前显示当前时间
    void ShowTime(IN bool bFlag);

    /// @brief 设置是否追加打开LOG档
    /// 默认新创建LOG档
    /// @param[in] bFlag true(追加方式打开LOG档) fale(新创建LOG档)
    void SetAppendOpen(IN bool bFlag);

    /// @brief 写一行LOG, 自动换行, 每行最多1024个字符
    /// @param[in] szFormat 需要写入LOG的格式化字符串
    void WriteLineW(IN const wchar_t* szFormat, ...);

    /// @brief 写一行LOG, 自动换行, 没行最多1024个字符
    /// @param[in] szFormat 需要写入LOG的格式化字符串
    void WriteLineA(IN const char* szFormat, ...);
};

/// @brief 打印LOG宏
#ifndef PrintLogA
#define PrintLogA(format, ...) LLog::WriteLineA(format, __VA_ARGS__)
#endif

/// @brief 打印LOG宏
#ifndef PrintLogW
#define PrintLogW(format, ...) LLog::WriteLineW(format, __VA_ARGS__)
#endif

#endif