

#ifndef _FTP_DOWNLOAD_
#define _FTP_DOWNLOAD_

#include <QtNetwork/QFtp>
#include <QtCore/QFile>



#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif


/// @brief 下载文件状态
struct DownloadState
{
    bool IsDone; ///< 标志下载是否完成
    qint64 DoneSize; ///< 完成大小
    qint64 TotalSize; ///< 文件总大小

    bool IsError; ///< 标志是否发生错误
    QString ErrorMsg; ///< 错误消息
};



/// @brief FTP下载类
class LFtpDownload : public QObject
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param[in] url 下载文件FTP URL
    explicit LFtpDownload(IN const QString& url);

    /// @brief 析构函数
    /// @return
    ~LFtpDownload();

    /// @brief 设置登陆信息
    /// @param[in] user 用户名
    /// @param[in] pwd 密码
    void SetLogin(IN const QString& user, IN const QString& pwd);

    /// @brief 设置保存文件的路径
    /// @param[in] path 路径
    /// @return
    void SetFilePath(IN const QString& path);

    /// @brief 开始下载
    /// 该方法为异步方法, 该方法必须运行在UI线程中
    void StartDownloadAsync();

    /// @brief 获取下载状态
    /// @return 下载状态
    const DownloadState& GetDownloadState();

private slots:
    /// @brief 下载完成槽函数
    /// @param[in] bError 标志是否发生错误
    void FtpDownloadDone(bool bError);

    /// @brief 数据传输进度
    /// @param[in] done 已完成字节数
    /// @param[in] total 总字节数
    void DataTransferProgress(qint64 done, qint64 total);

private:
    QFtp m_ftp; ///< FTP对象
    QFile m_downloadFile; ///< 保存的文件对象
    QString m_filePath; ///< 文件保存路径
    QString m_downloadUrl; ///< 下载链接

    QString m_loginUser; ///< FTP登录用户名
    QString m_loginPwd; ///< FTP登录密码

    DownloadState m_downloadState; ///< 存储下载状态
};

#endif