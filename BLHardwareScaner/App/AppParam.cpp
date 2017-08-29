
#include "AppParam.h"

#include <QtCore/QFile>


#define VERSION_FILE_PATH ".\\Config\\Version"
#define STRING_PARAM_RESTARTAGING "-RestartAging"


static APP_START_MODE s_appStartMode = APP_NORMAL; ///< 程序启动模式
static QString s_appVersion = ""; ///< 程序版本

/// @brief 解析程序启动模式
/// @param[in] argc 命令行参数数目
/// @param[in] argv[] 命令行参数
/// @return 程序启动模式
static APP_START_MODE ParseStartMode(int argc, char *argv[])
{
    APP_START_MODE startMode = APP_NORMAL;
    if (1 == argc)
    {
        startMode = APP_NORMAL;
    }

    if (2 == argc)
    {
        QString param = argv[1];
        if (param == STRING_PARAM_RESTARTAGING)
        {
            startMode = APP_RESTARTAGING;
        }
        else
        {
            startMode = APP_NORMAL;
        }
    }

    return startMode;
}

/// @brief 读取程序版本信息
/// @return 程序版本信息
static QString ReadAppVersion()
{
    QFile versionFile(VERSION_FILE_PATH);
    if (!versionFile.open(QIODevice::ReadOnly))
    {
        return "";
    }

    QString version = versionFile.readAll();
    versionFile.close();

    return version.trimmed();
}

APP_START_MODE LAppParam::GetStartMode()
{
    return s_appStartMode;
}

QString LAppParam::GetAppVersion()
{
    return s_appVersion;
}

QString LAppParam::GetDefaultQssPath()
{
    return ".\\Config\\QSS\\Default\\";
}

void LAppParam::InitAppParam(int argc, char *argv[])
{
    s_appStartMode = ParseStartMode(argc, argv);
    s_appVersion = ReadAppVersion();
}


