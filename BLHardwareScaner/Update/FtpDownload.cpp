
#include "FtpDownload.h"

#include <QtCore/QUrl>

#include <QtCore/QFileInfo>

LFtpDownload::LFtpDownload(IN const QString& url)
{
    m_downloadUrl = url;

    m_filePath = ".\\";

    m_downloadState.IsDone = true;
    m_downloadState.DoneSize = 0;
    m_downloadState.TotalSize = 1;
    m_downloadState.IsError = false;
    m_downloadState.ErrorMsg = "";

    connect(&m_ftp, SIGNAL(done(bool)), this, SLOT(FtpDownloadDone(bool)));
    connect(&m_ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(DataTransferProgress(qint64, qint64)));
}

LFtpDownload::~LFtpDownload()
{

}

void LFtpDownload::SetLogin(IN const QString& user, IN const QString& pwd)
{
    m_loginUser = user;
    m_loginPwd = pwd;
}

void LFtpDownload::SetFilePath(IN const QString& path)
{
    m_filePath = path;
}

void LFtpDownload::StartDownloadAsync()
{
    m_downloadState.IsDone = false;
    m_downloadState.DoneSize = 0;
    m_downloadState.TotalSize = 1;
    m_downloadState.IsError = false;
    m_downloadState.ErrorMsg = "";

    QUrl url(m_downloadUrl);

    QString fileName = QFileInfo(url.path()).fileName();
    m_downloadFile.setFileName(m_filePath + fileName);
    m_downloadFile.open(QIODevice::WriteOnly);

    m_ftp.connectToHost(url.host(), url.port(21));

    m_ftp.login(m_loginUser, m_loginPwd);

    m_ftp.get(url.path(), &m_downloadFile);

    m_ftp.close();
}

void LFtpDownload::FtpDownloadDone(bool bError)
{
    m_downloadFile.close();

    m_downloadState.IsDone = true;

    if (bError)
    {
        m_downloadState.IsError = true;
        m_downloadState.ErrorMsg = m_ftp.errorString();
    }
}

void LFtpDownload::DataTransferProgress(IN qint64 done, IN qint64 total)
{
    m_downloadState.TotalSize = total;
    m_downloadState.DoneSize = done;
}

const DownloadState& LFtpDownload::GetDownloadState()
{
    return m_downloadState;
}