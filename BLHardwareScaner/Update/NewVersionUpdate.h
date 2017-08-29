

#ifndef _NEWVERSION_UPDATE_H_
#define _NEWVERSION_UPDATE_H_

#include "FtpDownload.h"

/// @brief 检查新版本结果
enum CHECKNEW_RESULT
{
    CHECKNEW_UNKNOWN = 0, ///< 未知
    CHECKNEW_RUNNING, ///< 正在运行
    CHECKNEW_WITH_ERROR, ///< 有错误
    CHECKNEW_WITH_NEW, ///< 有新的更新
    CHECKNEW_WITHOUT_NEW ///< 没有新的更新
};

/// @brief 检查新版本
class CheckNew
{
public:
    /// @brief 构造函数
    CheckNew();

    /// @brief 析构函数
    ~CheckNew();

    /// @brief 设置当前版本, 格式: V1.1.1
    /// @param[in] version 当前版本用于比较
    void SetCurrentVersion(const QString& version);

    /// @brief 开始检查新版本
    /// 该方法为异步方法
    /// 该方法必须运行在UI线程中
    void StartCheckAsync();

    /// @brief 获取结果
    /// @return 检查新版本结果
    CHECKNEW_RESULT GetResult();

private:
    QString m_currentVersion; ///< 当前版本, 格式: V1.1.1
    LFtpDownload* m_pFtpDownload; ///< FTP下载对象
};


// 下载错误
#define DOWNLOADNEW_ERROR -1

// 下载完成
#define DOWNLOADNEW_DONE 101

/// @brief 下载新版本
class DownloadNew
{
public:
    /// @brief 构造函数
    DownloadNew();

    /// @brief 析构函数
    ~DownloadNew();

    /// @brief 开始下载新版本
    /// 该方法为异步方法
    /// 该方法必须运行在UI线程中
    void StartDownloadAsync();

    /// @brief 获取结果
    /// 返回值[0, 100]表示正在下载的下载进度, 返回值为-1则表示发生错误, 返回值为101则表示下载完成
    /// @return 下载新版本结果
    int GetResult();

private:
    LFtpDownload* m_pFtpDownload; ///< FTP下载对象
};

#endif