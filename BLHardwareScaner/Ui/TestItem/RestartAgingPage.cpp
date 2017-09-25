
#include "RestartAgingPage.h"

#include <QtGui/QValidator>
#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include <Windows.h>

#include "App\\AppParam.h"

#include "..\\Src\\LHardwareInfor.h"
#include "..\\Src\\Log\\LLog.h"
#include "..\\Src\\SetupApi\\LSetupAPI.h"
#include "..\\Src\\PowerMgmt\\PowerMgmt.h"
#include "..\\Src\\TaskScheduler\\LTaskScheduler.h"

// 默认间隔时间60秒
#define DEFAULT_INTERVALTIME 60

// 默认Aging次数60次
#define DEFAULT_AGINGTIMES 60

#define STRING_START_TEST QObject::tr("StartTest")
#define STRING_STOP_TEST QObject::tr("StopTest")

#define RESTARTAGING_TTEST_CONFIG_PATH ".\\Temp\\TestItem\\"
#define RESTARTAGING_TTEST_CONFIG_FILE ".\\Temp\\TestItem\\RestartAging.ini"
#define RESTARTAGING_TEST_DEVICE_FILE ".\\Temp\\TestItem\\RestartAgingDevices.txt"
#define RESTARTAGING_TEST_LOSEDEVICE_FILE ".\\Temp\\TestItem\\RestartAgingLoseDevices.txt"

#define RESTARTAGING_ACTION "Test/Action"
#define RESTARTAGING_INTERVALTIMES "Test/IntervalTimes"
#define RESTARTAGING_AGINGTIMES "Test/AgingTimes"
#define RESTARTAGING_COMPLETETIMES "Test/CompleteTimes"
#define RESTARTAGING_BSODTIMES "Test/BSODTimes"
#define RESTARTAGING_LASTRESTARTTIME "Test/LastRestartTime"
#define RESTARTAGING_DEVICELOSE "Test/DeviceLose"


#define RESTART_TASK_NAME L"BLHWScaner RestartAging"

#define RESTARTAGING_DATA_FORMAT "yyyy-MM-dd hh:mm:ss"

#define ACTION_RST "RST"
#define ACTION_S3 "S3"
#define ACTION_S4 "S4"

RestartAgingPage::RestartAgingPage(IN QWidget *parent /* = 0 */, IN Qt::WFlags flags /* = 0 */)
    : QDialog(parent, flags)
{
    ui.setupUi(this);
    this->LoadQSS();

    QObject::connect(ui.pushButtonTest, SIGNAL(clicked()), this, SLOT(TestButtonClicked()));
    QObject::connect(ui.pushButtonReset, SIGNAL(clicked()), this, SLOT(ResetButtonClicked()));
    QObject::connect(&m_countDownTimer, SIGNAL(timeout()), this, SLOT(CountDownTimerTimeout()));
    QObject::connect(ui.checkBoxRestart, SIGNAL(clicked()), this, SLOT(CheckBoxClicked()));
    QObject::connect(ui.checkBoxS3, SIGNAL(clicked()), this, SLOT(CheckBoxClicked()));
    QObject::connect(ui.checkBoxS4, SIGNAL(clicked()), this, SLOT(CheckBoxClicked()));
	QObject::connect(QApplication::instance(),SIGNAL(ResumeSuspend()),this,SLOT(ResumeSuspend()));  

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
    ui.labelDeviceLose->setText("0");

    ui.pushButtonTest->setText(STRING_START_TEST);


    m_countDownTimer.setInterval(1000);
    m_currentCountDown = DEFAULT_INTERVALTIME;

	// 将CheckBox设置为一组, 设置为一组的CheckBox只能单选
	QButtonGroup* pGroup = new QButtonGroup(this);
	pGroup->addButton(ui.checkBoxRestart, 0);
	pGroup->addButton(ui.checkBoxS3, 1);
	pGroup->addButton(ui.checkBoxS4, 2);
	
	ui.checkBoxRestart->setChecked(true);

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
        QList<QString> loseDevices;
        if (this->IsDeviceLoseHappened(loseDevices))
        {
            this->SaveLoseDevicesToFile(loseDevices);
            testParam.DeviceLose += 1;
        }
        QDateTime currentDateTime = QDateTime::currentDateTime();
        testParam.LastRestartTime = currentDateTime.toString(RESTARTAGING_DATA_FORMAT);
        this->SaveTestParamToFile(testParam);

        this->SetUITestParam(testParam);
        

        if (testParam.CompletedTimes < testParam.AgingTimes)
        {
            this->SetUITesting();

            m_currentCountDown = testParam.IntervalTime;
            m_countDownTimer.start();
        }
        else
        {
            this->DeleteRestartTaskScheduler();
        }
    }
}

void RestartAgingPage::TestButtonClicked()
{
    if (ui.pushButtonTest->text() == STRING_START_TEST)
    {
        this->SetUITesting();

        m_currentCountDown = ui.lineEditIntervalTime->text().toInt();
        m_countDownTimer.start();

        this->RegisterRestartTaskScheduler();

		// 保存测试参数
		RestartAgingParam testParam;
		testParam.IntervalTime = ui.lineEditIntervalTime->text().toInt();
		testParam.AgingTimes = ui.lineEditAgingTimes->text().toInt();
		testParam.CompletedTimes = ui.labelCompletedTimes->text().toInt();
		testParam.BSODTimes = ui.labelBSODTimes->text().toInt();
        testParam.DeviceLose = ui.labelDeviceLose->text().toInt();
		QDateTime currentDateTime = QDateTime::currentDateTime();
		testParam.LastRestartTime = currentDateTime.toString(RESTARTAGING_DATA_FORMAT);
		if (ui.checkBoxRestart->checkState() == Qt::Checked)
			testParam.Action = ACTION_RST;
		else if (ui.checkBoxS3->checkState() == Qt::Checked)
			testParam.Action = ACTION_S3;
		else if (ui.checkBoxS4->checkState() == Qt::Checked)
			testParam.Action = ACTION_S4;

		this->SaveTestParamToFile(testParam);

        // 将当前所有设备信息存储起来
        QSet<QString> devicesList;
        this->GetCurrentDevices(devicesList);
        this->SaveDevicesToFile(devicesList);

        this->DeleteLoseDevicesFile();

        return;
    }

    if (ui.pushButtonTest->text() == STRING_STOP_TEST)
    {
        this->SetUINoTesting();

        m_countDownTimer.stop();

        this->DeleteRestartTaskScheduler();

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
    ui.labelDeviceLose->setText("0");
}

void RestartAgingPage::CheckBoxClicked()
{
    this->ResetButtonClicked();
}

void RestartAgingPage::CountDownTimerTimeout()
{
    ui.labelCountDown->setText(QObject::tr("%1").arg(m_currentCountDown));

    if (0 == m_currentCountDown)
    {
        m_countDownTimer.stop();

		if (ui.checkBoxS4->checkState() == Qt::Checked)
            PowerMgmt::EnterHibernate();
		else if (ui.checkBoxS3->checkState() == Qt::Checked)
            PowerMgmt::EnterSleep();
		else if (ui.checkBoxRestart->checkState() == Qt::Checked)
			system("shutdown -r -f -t 0");
        return;
    }

    m_currentCountDown = m_currentCountDown - 1;

}

void RestartAgingPage::ResumeSuspend()
{
	PrintLogA("RestartAgingPage: Resume From S3 Or S4");

	// 从S3或S4状态中恢复

	// 如果没有进行S3或S4测试，则直接退出
	if (!this->IsS3S4Running())
		return;

	// 如果测试定时器已经运行则直接退出
	if (m_countDownTimer.isActive())
		return;

	// 恢复测试

	// 从配置文件中读取信息
	RestartAgingParam testParam;
	this->GetTestParamFromFile(testParam);

	testParam.CompletedTimes += 1;

	if (this->IsBSODHappened(testParam.LastRestartTime))
	{
		testParam.BSODTimes += 1;
	}
     QList<QString> loseDevices;
    if (this->IsDeviceLoseHappened(loseDevices))
    {
        testParam.DeviceLose += 1;
        this->SaveLoseDevicesToFile(loseDevices);
    }

    // 进S3或S4前前保存测试参数
    QDateTime currentDateTime = QDateTime::currentDateTime();
    testParam.LastRestartTime = currentDateTime.toString(RESTARTAGING_DATA_FORMAT);
    this->SaveTestParamToFile(testParam);

    this->SetUITestParam(testParam);


	if (testParam.CompletedTimes < testParam.AgingTimes)
	{
		m_currentCountDown = testParam.IntervalTime;
		m_countDownTimer.start();
	}
	else
	{
        this->SetUINoTesting();
        this->DeleteRestartTaskScheduler();
        m_countDownTimer.stop();
	}
}

void RestartAgingPage::SetUITesting()
{
    ui.pushButtonTest->setText(STRING_STOP_TEST);
    ui.lineEditIntervalTime->setEnabled(false);
    ui.lineEditAgingTimes->setEnabled(false);
    ui.pushButtonReset->setEnabled(false);
    ui.checkBoxRestart->setEnabled(false);
    ui.checkBoxS3->setEnabled(false);
    ui.checkBoxS4->setEnabled(false);
}

void RestartAgingPage::SetUINoTesting()
{
    ui.pushButtonTest->setText(STRING_START_TEST);
    ui.lineEditIntervalTime->setEnabled(true);
    ui.lineEditAgingTimes->setEnabled(true);
    ui.pushButtonReset->setEnabled(true);
    ui.checkBoxRestart->setEnabled(true);
    ui.checkBoxS3->setEnabled(true);
    ui.checkBoxS4->setEnabled(true);
}

void RestartAgingPage::SetUITestParam(const RestartAgingParam& param)
{
    ui.lineEditIntervalTime->setText(QObject::tr("%1").arg(param.IntervalTime));
    ui.lineEditAgingTimes->setText(QObject::tr("%1").arg(param.AgingTimes));
    ui.labelCompletedTimes->setText(QObject::tr("%1").arg(param.CompletedTimes));
    ui.labelBSODTimes->setText(QObject::tr("%1").arg(param.BSODTimes));
    ui.labelDeviceLose->setText(QObject::tr("%1").arg(param.DeviceLose));
    if (param.Action == ACTION_RST)
        ui.checkBoxRestart->setChecked(true);
    else if (param.Action == ACTION_S3)
        ui.checkBoxS3->setChecked(true);
    else if (param.Action == ACTION_S4)
        ui.checkBoxS4->setChecked(true);
}

bool RestartAgingPage::IsS3S4Running()
{
	if (ui.pushButtonTest->text() == STRING_STOP_TEST)
	{
		if (ui.checkBoxS3->checkState() == Qt::Checked ||
			ui.checkBoxS4->checkState() == Qt::Checked)
		{
			return true;
		}
	}

	return false;
	
}

void RestartAgingPage::RegisterRestartTaskScheduler()
{
    LTaskScheduler taskScheduler;
    taskScheduler.Principal(true);
    taskScheduler.Trigger(2, NULL);
    taskScheduler.SetStartOnBattery(true);

    QString appFilePath = QCoreApplication::applicationFilePath();
    taskScheduler.Action(appFilePath.toStdWString().c_str(), L"-RestartAging", NULL);
    taskScheduler.Register(NULL, RESTART_TASK_NAME);
}

void RestartAgingPage::DeleteRestartTaskScheduler()
{
    LTaskScheduler::Delete(NULL, RESTART_TASK_NAME);
}

void RestartAgingPage::SaveTestParamToFile(IN const RestartAgingParam& param)
{
    QSettings* pConfigIniWrite = new QSettings(RESTARTAGING_TTEST_CONFIG_FILE, QSettings::IniFormat);   
    pConfigIniWrite->setValue(RESTARTAGING_INTERVALTIMES, param.IntervalTime);    
    pConfigIniWrite->setValue(RESTARTAGING_AGINGTIMES, param.AgingTimes);  
    pConfigIniWrite->setValue(RESTARTAGING_COMPLETETIMES, param.CompletedTimes);  
    pConfigIniWrite->setValue(RESTARTAGING_BSODTIMES, param.BSODTimes);
    pConfigIniWrite->setValue(RESTARTAGING_LASTRESTARTTIME, param.LastRestartTime);
	pConfigIniWrite->setValue(RESTARTAGING_ACTION, param.Action);
    pConfigIniWrite->setValue(RESTARTAGING_DEVICELOSE, param.DeviceLose);
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
	param.Action = pConfigIniRead->value(RESTARTAGING_ACTION, "").toString();
    param.DeviceLose = pConfigIniRead->value(RESTARTAGING_DEVICELOSE, 0).toInt();

    if (NULL != pConfigIniRead)
    {
        delete pConfigIniRead;
        pConfigIniRead = NULL;
    }
}

void RestartAgingPage::GetCurrentDevices(OUT QSet<QString>& devicesList)
{
    devicesList.clear();

    LSetupDevAll devAll;
    for (int i = 0; i < devAll.GetDevNum(); i++)
    {
        wstring id;
        devAll.GetInstanceID(i, id);
        devicesList.insert(QString::fromStdWString(id).trimmed());
    }

}

void RestartAgingPage::SaveDevicesToFile(IN const QSet<QString>& devicesList)
{
    QFile file(RESTARTAGING_TEST_DEVICE_FILE);  
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;  

    QTextStream out(&file);
    for (auto iter = devicesList.begin(); iter != devicesList.end(); iter++)
    {
        out << (*iter) << "\n"; 
    }
     
}

void RestartAgingPage::GetDevicesFromFile(OUT QSet<QString>& devicesList)
{
    devicesList.clear();

    QFile file(RESTARTAGING_TEST_DEVICE_FILE);  
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  
        return;  

    QTextStream in(&file);  
    QString line = in.readLine();  
    while (!line.isNull()) {  
        devicesList.insert(line.trimmed());
        line = in.readLine();  
    }  
}

void RestartAgingPage::SaveLoseDevicesToFile(IN const QList<QString>& loseDevices)
{
    QFile file(RESTARTAGING_TEST_LOSEDEVICE_FILE);  
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;  

    QTextStream out(&file);
    for (auto iter = loseDevices.begin(); iter != loseDevices.end(); iter++)
    {
        out << (*iter) << "\n"; 
        out << "\n"; 
    }
}

void RestartAgingPage::DeleteLoseDevicesFile()
{
    QFile::remove(RESTARTAGING_TEST_LOSEDEVICE_FILE);
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

bool RestartAgingPage::IsDeviceLoseHappened(OUT QList<QString>& loseDevices)
{
    bool bRet = false;

    loseDevices.clear();

    QSet<QString> currentDevsList;
    this->GetCurrentDevices(currentDevsList);

    QSet<QString> oldDevsList;
    this->GetDevicesFromFile(oldDevsList);

    for (auto iter = oldDevsList.begin(); iter != oldDevsList.end(); iter++)
    {
        if (!currentDevsList.contains(*iter))
        {
            bRet = true;
            loseDevices.append(*iter);
        }
    }

    return bRet;
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