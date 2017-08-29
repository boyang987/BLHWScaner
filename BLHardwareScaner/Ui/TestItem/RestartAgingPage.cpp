
#include "RestartAgingPage.h"

#include <QtGui/QValidator>
#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>

#include "App\\AppParam.h"

#include "..\\Src\\LHardwareInfor.h"
#include "..\\Src\\TaskScheduler\\LTaskScheduler.h"

// 默认间隔时间60秒
#define DEFAULT_INTERVALTIME 60

// 默认Aging次数60次
#define DEFAULT_AGINGTIMES 60

#define STRING_START_TEST QObject::tr("StartTest")
#define STRING_STOP_TEST QObject::tr("StopTest")

#define RESTARTAGING_TTEST_CONFIG_PATH ".\\Temp\\TestItem\\"
#define RESTARTAGING_TTEST_CONFIG_FILE ".\\Temp\\TestItem\\RestartAging.ini"

#define RESTARTAGING_INTERVALTIMES "Test/IntervalTimes"
#define RESTARTAGING_AGINGTIMES "Test/AgingTimes"
#define RESTARTAGING_COMPLETETIMES "Test/CompleteTimes"
#define RESTARTAGING_BSODTIMES "Test/BSODTimes"
#define RESTARTAGING_LASTRESTARTTIME "Test/LastRestartTime"

#define RESTARTAGING_TASK_NAME L"BLHWScaner RestartAging"

#define RESTARTAGING_DATA_FORMAT "yyyy-MM-dd hh:mm:ss"


RestartAgingPage::RestartAgingPage(IN QWidget *parent /* = 0 */, IN Qt::WFlags flags /* = 0 */)
    : QDialog(parent, flags)
{
    ui.setupUi(this);
    this->LoadQSS();

    QObject::connect(ui.pushButtonTest, SIGNAL(clicked()), this, SLOT(TestButtonClicked()));
    QObject::connect(ui.pushButtonReset, SIGNAL(clicked()), this, SLOT(ResetButtonClicked()));
    QObject::connect(&m_countDownTimer, SIGNAL(timeout()), this, SLOT(CountDownTimerTimeout()));

    // 设置输入框只接受[0, 999]之间的数字
    // 因为设置pValidator的parent为this, 所以可以不delete, 因为this被delete时, pValidator也会被delete
    QValidator* pValidator = new QIntValidator(0, 999, this);
    ui.lineEditIntervalTime->setValidator(pValidator);
    ui.lineEditAgingTimes->setValidator(pValidator);

    ui.lineEditIntervalTime->setText(QObject::tr("%1").arg(DEFAULT_INTERVALTIME));
    ui.lineEditAgingTimes->setText(QObject::tr("%1").arg(DEFAULT_AGINGTIMES));

    ui.labelCountDown->setText(QObject::tr("%1").arg(DEFAULT_INTERVALTIME));
    ui.labelCompletedTimes->setText("0");
    ui.labelBSODTimes->setText("0");

    ui.pushButtonTest->setText(STRING_START_TEST);


    m_countDownTimer.setInterval(1000);
    m_currentCountDown = DEFAULT_INTERVALTIME;

    this->CheckPath(RESTARTAGING_TTEST_CONFIG_PATH);
}

RestartAgingPage::~RestartAgingPage()
{

}

void RestartAgingPage::showEvent(QShowEvent* e)
{
    static bool sFristShow = true;
    if (!sFristShow)
        return;


    sFristShow = false;

    if (APP_NORMAL == LAppParam::GetStartMode())
        return;

    if (APP_RESTARTAGING == LAppParam::GetStartMode())
    {
        // 从配置文件中读取信息
        RestartAgingParam testParam;
        this->GetTestParamFromFile(testParam);

        testParam.CompletedTimes += 1;

        if (this->IsBSODHappened(testParam.LastRestartTime))
        {
            testParam.BSODTimes += 1;
        }

        ui.lineEditIntervalTime->setText(QObject::tr("%1").arg(testParam.IntervalTime));
        ui.lineEditAgingTimes->setText(QObject::tr("%1").arg(testParam.AgingTimes));
        ui.labelCompletedTimes->setText(QObject::tr("%1").arg(testParam.CompletedTimes));
        ui.labelBSODTimes->setText(QObject::tr("%1").arg(testParam.BSODTimes));

        if (testParam.CompletedTimes < testParam.AgingTimes)
        {
            this->TestButtonClicked();
        }
        else
        {
            this->DeleteTaskScheduler();
        }
    }
}

void RestartAgingPage::TestButtonClicked()
{
    if (ui.pushButtonTest->text() == STRING_START_TEST)
    {
        ui.pushButtonTest->setText(STRING_STOP_TEST);
        ui.lineEditIntervalTime->setEnabled(false);
        ui.lineEditAgingTimes->setEnabled(false);
        ui.pushButtonReset->setEnabled(false);

        m_currentCountDown = ui.lineEditIntervalTime->text().toInt();
        m_countDownTimer.start();

        this->RegisterTaskScheduler();

        return;
    }

    if (ui.pushButtonTest->text() == STRING_STOP_TEST)
    {
        ui.pushButtonTest->setText(STRING_START_TEST);
        ui.lineEditIntervalTime->setEnabled(true);
        ui.lineEditAgingTimes->setEnabled(true);
        ui.pushButtonReset->setEnabled(true);

        m_countDownTimer.stop();

        this->DeleteTaskScheduler();

        return;
    }
}

void RestartAgingPage::ResetButtonClicked()
{
    ui.lineEditIntervalTime->setText(QObject::tr("%1").arg(DEFAULT_INTERVALTIME));
    ui.lineEditAgingTimes->setText(QObject::tr("%1").arg(DEFAULT_AGINGTIMES));

    ui.labelCountDown->setText(QObject::tr("%1").arg(DEFAULT_INTERVALTIME));
    ui.labelCompletedTimes->setText("0");
    ui.labelBSODTimes->setText("0");
}

void RestartAgingPage::CountDownTimerTimeout()
{
    ui.labelCountDown->setText(QObject::tr("%1").arg(m_currentCountDown));

    if (0 == m_currentCountDown)
    {
        m_countDownTimer.stop();

        // 重启前保存测试参数
        RestartAgingParam testParam;
        testParam.IntervalTime = ui.lineEditIntervalTime->text().toInt();
        testParam.AgingTimes = ui.lineEditAgingTimes->text().toInt();
        testParam.CompletedTimes = ui.labelCompletedTimes->text().toInt();
        testParam.BSODTimes = ui.labelBSODTimes->text().toInt();
        QDateTime currentDateTime = QDateTime::currentDateTime();
        testParam.LastRestartTime = currentDateTime.toString(RESTARTAGING_DATA_FORMAT);

        this->SaveTestParamToFile(testParam);

        system("shutdown -r -f -t 0");
        return;
    }


    
    m_currentCountDown = m_currentCountDown - 1;

}

void RestartAgingPage::RegisterTaskScheduler()
{
    LTaskScheduler taskScheduler;
    taskScheduler.Principal(true);
    taskScheduler.Trigger(2);
    taskScheduler.Settings(true);

    QString appFilePath = QCoreApplication::applicationFilePath();
    taskScheduler.Action(appFilePath.toStdWString().c_str(), L"-RestartAging", NULL);
    taskScheduler.Register(NULL, RESTARTAGING_TASK_NAME);
}

void RestartAgingPage::DeleteTaskScheduler()
{
    LTaskScheduler::Delete(NULL, RESTARTAGING_TASK_NAME);
}

void RestartAgingPage::SaveTestParamToFile(IN const RestartAgingParam& param)
{
    QSettings* pConfigIniWrite = new QSettings(RESTARTAGING_TTEST_CONFIG_FILE, QSettings::IniFormat);   
    pConfigIniWrite->setValue(RESTARTAGING_INTERVALTIMES, param.IntervalTime);    
    pConfigIniWrite->setValue(RESTARTAGING_AGINGTIMES, param.AgingTimes);  
    pConfigIniWrite->setValue(RESTARTAGING_COMPLETETIMES, param.CompletedTimes);  
    pConfigIniWrite->setValue(RESTARTAGING_BSODTIMES, param.BSODTimes);
    pConfigIniWrite->setValue(RESTARTAGING_LASTRESTARTTIME, param.LastRestartTime);
    if (NULL != pConfigIniWrite)
    {
        delete pConfigIniWrite;
        pConfigIniWrite = NULL;
    }
}

void RestartAgingPage::GetTestParamFromFile(OUT RestartAgingParam& param)
{
    QSettings* pConfigIniRead = new QSettings(RESTARTAGING_TTEST_CONFIG_FILE, QSettings::IniFormat);   
    param.IntervalTime =  pConfigIniRead->value(RESTARTAGING_INTERVALTIMES, DEFAULT_INTERVALTIME).toInt();    
    param.AgingTimes = pConfigIniRead->value(RESTARTAGING_AGINGTIMES, DEFAULT_AGINGTIMES).toInt();  
    param.CompletedTimes = pConfigIniRead->value(RESTARTAGING_COMPLETETIMES, 0).toInt();  
    param.BSODTimes = pConfigIniRead->value(RESTARTAGING_BSODTIMES, 0).toInt();
    param.LastRestartTime = pConfigIniRead->value(RESTARTAGING_LASTRESTARTTIME, "").toString();
    if (NULL != pConfigIniRead)
    {
        delete pConfigIniRead;
        pConfigIniRead = NULL;
    }
}

bool RestartAgingPage::IsBSODHappened(IN const QString& lastRestartTime)
{
    if (lastRestartTime.isEmpty())
        return false;

    // 获取当前时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTime = currentDateTime.toString(RESTARTAGING_DATA_FORMAT);

    // 获取Windows目录
    OperatingSystemInfor osInfor = LHardwareInfor::GetOperatingSystemInfor();
    QString windowsPath = QString::fromStdWString(osInfor.SystemDrive);
    windowsPath += "\\Windows\\";

    // 检查是否存在完整DUMP文件
    QString dumpFile = windowsPath + "MEMORY.DMP";
    if (QFile::exists(dumpFile))
    {
        QFileInfo fileInfo(dumpFile);
        QDateTime fileDateTime = fileInfo.lastModified();
        QString fileTime = fileDateTime.toString(RESTARTAGING_DATA_FORMAT);

        // 修改日期在上次重启后, 则表示发生蓝屏
        if (fileTime >= lastRestartTime &&
            fileTime <= currentTime)
        {
            return true;
        }
    }

    // 检查Minidump文件
    QString minidumpPath = windowsPath + "Minidump";
    QDir minidumpDir(minidumpPath);
    if (!minidumpDir.exists())
        return false;

    minidumpDir.setFilter(QDir::Files);
    QFileInfoList fileList = minidumpDir.entryInfoList();
    for (int i = 0; i < fileList.size(); i++)
    {
        QFileInfo fileInfo = fileList[i];
        if (!fileInfo.fileName().contains(".dmp", Qt::CaseInsensitive))
            continue;

        QDateTime fileDateTime = fileInfo.lastModified();
        QString fileTime = fileDateTime.toString(RESTARTAGING_DATA_FORMAT);

        // 修改日期在上次重启后, 则表示发生蓝屏
        if (fileTime >= lastRestartTime &&
            fileTime <= currentTime)
        {
            return true;
        }

    }


    return false;
}

bool RestartAgingPage::CheckPath(IN const QString& qstrPath)
{
    QDir logDir;
    if (!logDir.exists(qstrPath))
    {
        return logDir.mkpath(qstrPath);
    }

    return true;
}

void RestartAgingPage::LoadQSS()
{
    QString filePath = LAppParam::GetDefaultQssPath();
    filePath += "RestartAgingPage.qss";
    QFile qssFile(filePath);   
    qssFile.open(QFile::ReadOnly);  

    if(qssFile.isOpen())  
    {  
        QString qss = qssFile.readAll();
        qssFile.close();  
        this->setStyleSheet(qss);
    } 
}