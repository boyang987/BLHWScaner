
#include "NewVersionUpdate.h"

#include <QtCore/QDir>
#include <QtCore/QFile>



#define FTP_USER "bxu2359840588"
#define FTP_PWD ""

#define CHECK_NEW_URL "ftp://bxu2359840588@bxu2359840588.my3w.com/myfolder/SoftRelease/BLHWScaner/Version"
#define DOWNLOAD_NEW_URL   "ftp://bxu2359840588@bxu2359840588.my3w.com/myfolder/SoftRelease/BLHWScaner/BLHWScaner.rar"

#define UPDATE_PATH ".\\Temp\\Update\\"
#define CHECK_NEW_FILE_PATH ".\\Temp\\Update\\Version"

/// @brief 检查路径, 路径不存在则创建
/// @param[in] qstrPath 路径
/// @return 成功返回true, 失败返回false
static bool CheckPath(IN const QString& qstrPath)
{
    QDir logDir;
    if (!logDir.exists(qstrPath))
    {
        return logDir.mkpath(qstrPath);
    }

    return true;
}

CheckNew::CheckNew()
{
    m_pFtpDownload = new LFtpDownload(CHECK_NEW_URL);
    m_pFtpDownload->SetLogin(FTP_USER, FTP_PWD);
    m_pFtpDownload->SetFilePath(UPDATE_PATH);
}

CheckNew::~CheckNew()
{
    if (NULL != m_pFtpDownload)
    {
        delete m_pFtpDownload;
        m_pFtpDownload = NULL;
    }
}

void CheckNew::SetCurrentVersion(const QString& version)
{
    m_currentVersion = version;
}

void CheckNew::StartCheckAsync()
{
    CheckPath(UPDATE_PATH);

    // 开始下载版本文件
    m_pFtpDownload->StartDownloadAsync();
}

CHECKNEW_RESULT CheckNew::GetResult()
{
    DownloadState state = m_pFtpDownload->GetDownloadState();
    if (!state.IsDone)
    {
        return CHECKNEW_RUNNING;
    }

    if (state.IsError)
    {
        QFile::remove(CHECK_NEW_FILE_PATH);
        return CHECKNEW_WITH_ERROR;
    }

    QFile checkNewFile(CHECK_NEW_FILE_PATH);
    if (!checkNewFile.open(QIODevice::ReadOnly))
    {
        return CHECKNEW_WITH_ERROR;
    }

    QString version = checkNewFile.readAll();
    checkNewFile.close();
    checkNewFile.remove();


    version = version.trimmed();

    int cmp = m_currentVersion.compare(version);
    if (cmp >= 0)
    {
        return CHECKNEW_WITHOUT_NEW;
    }

    if (cmp < 0)
    {
        return CHECKNEW_WITH_NEW;
    }

    return CHECKNEW_UNKNOWN;
}

DownloadNew::DownloadNew()
{
    m_pFtpDownload = new LFtpDownload(DOWNLOAD_NEW_URL);
    m_pFtpDownload->SetLogin(FTP_USER, FTP_PWD);
    m_pFtpDownload->SetFilePath(UPDATE_PATH);
}

DownloadNew::~DownloadNew()
{
    if (NULL != m_pFtpDownload)
    {
        delete m_pFtpDownload;
        m_pFtpDownload = NULL;
    }
}

void DownloadNew::StartDownloadAsync()
{
    CheckPath(UPDATE_PATH);

    // 开始下载版本文件
    m_pFtpDownload->StartDownloadAsync();
}

int DownloadNew::GetResult()
{
    DownloadState state = m_pFtpDownload->GetDownloadState();
    if (!state.IsDone)
    {
        int iRet = state.DoneSize * 100 /state.TotalSize;
        return iRet;
    }

    if (state.IsError)
    {
        return DOWNLOADNEW_ERROR;
    }

    return DOWNLOADNEW_DONE;
}