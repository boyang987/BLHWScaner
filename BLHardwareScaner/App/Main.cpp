
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "Ui\\MainPage.h"
#include "App\\AppParam.h"
#include "..\\Src\\Log\\LLog.h"
#include "..\\Src\\LAppInstance.h"


#define LOG_PATH ".\\Temp\\Log"

// 应用程序实例名称
#define APP_INSTANCE_NAME L"Global\\BLHWScaner{EB83918F-DAFD-47E6-A363-9B874F2CF2AF}"

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

/// @brief 检查更新安装程序的可执行文件
/// 如果有新的则删除旧文件, 重命名新文件
/// unrar.dllN变更为unrar.dll
/// UpdateInstall.exeN变更为UpdateInstall.exe
static void CheckUpdateInstallFile()
{
    bool bRet = QFile::exists(".\\Update\\unrar.dllN");
    if (bRet)
    {
        QFile::remove(".\\Update\\unrar.dll");
        QFile::rename(".\\Update\\unrar.dllN", ".\\Update\\unrar.dll");
    }

    bRet = QFile::exists(".\\Update\\UpdateInstall.exeN");
    if (bRet)
    {
        QFile::remove(".\\Update\\UpdateInstall.exe");
        QFile::rename(".\\Update\\UpdateInstall.exeN", ".\\Update\\UpdateInstall.exe");
    }
}

int main(int argc, char *argv[])
{
    // 检测程序是否已经在运行, 如果已经有一个实例在运行, 则退出当前实例
    LAppInstance appInstance(APP_INSTANCE_NAME);
    if (appInstance.IsAlreadyExist())
        return 0;

    QApplication app(argc, argv);

    // 不管程序以什么样的方式运行, 都设置程序的工作路径为程序本身所在的路径
    QString appPath = app.applicationDirPath();
    QDir::setCurrent(appPath);

    // 初始化APP启动模式
    LAppParam::InitAppParam(argc, argv);
    APP_START_MODE startMode = LAppParam::GetStartMode();

    CheckPath(LOG_PATH);
    // 如果正在进行重开机AGING测试, 则以追加方式打开LOG
    if (APP_RESTARTAGING == startMode)
    {
        LLog::SetAppendOpen(true);
    }
    LLog::ShowThreadId(true);
    LLog::ShowTime(true);
    LLog::Open(L".\\Temp\\Log\\TraceLog.txt");

    PrintLogW(L"Log Start");
    for (int i = 0; i < argc; i++)
    {
        PrintLogA("Command Argument%d: %s", i, argv[i]);
    }
    if (APP_NORMAL == startMode)
    {
        PrintLogW(L"AppStartMode: APP_NORMAL");
    }
    if (APP_RESTARTAGING == startMode)
    {
        PrintLogW(L"AppStartMode: APP_RESTARTAGING");
    }

    // 检查更新安装可执行文件
    CheckUpdateInstallFile();
   

    // 定义主页面
    MainPage mainPage;

    // 显示主页面
    mainPage.show();

    // 进入消息循环
    app.exec();

    // 关闭LOG
    PrintLogW(L"Log End");
    LLog::Close();

    // 程序结束
    return 0;
}