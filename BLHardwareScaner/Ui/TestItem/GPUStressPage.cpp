
#include "GPUStressPage.h"

#include <Windows.h>

#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtGui/QListView>

#include "App\\AppParam.h"

#include "..\\Src\\LHardwareInfor.h"


#define GPU_TEST_START QString::fromStdWString(L"Test Start")
#define GPU_TEST_STOP QString::fromStdWString(L"Test Stop")

#define TIME_5M   "5M"    // 5分钟
#define TIME_15M  "15M"   // 15分钟
#define TIME_30M  "30M"   // 30分钟
#define TIME_1H   "1H"    // 1小时
#define TIME_2H   "2H"    // 2小时
#define TIME_4H   "4H"    // 4小时
#define TIME_8H   "8H"    // 8小时
#define TIME_16H  "16H"   // 16小时

#define GPU_TTEST_CONFIG_FILE L".\\GPUStress\\ExeParam.ini"



GPUStressPage::GPUStressPage(IN float uiRatio, IN QWidget *parent /* = 0 */, IN Qt::WFlags flags /* = 0 */)
    : QDialog(parent, flags)
{
    m_gpuProcess.setWorkingDirectory(".\\GPUStress\\");
    m_gpuTimer.setInterval(500);
    QObject::connect(&m_gpuTimer, SIGNAL(timeout()), this, SLOT(GpuStressTimerTimeout()));
    

    ui.setupUi(this);
    this->LoadQSS(uiRatio);

    QObject::connect(ui.startTestButton, SIGNAL(clicked()), this, SLOT(TestButtonClicked()));

    const VideoCardInforArray& videoCards = LHardwareInfor::GetVideoCardInfor();
    for (unsigned int i = 0; i < videoCards.Count; i++)
    {
        ui.deviceComboBox->addItem(QString::fromStdWString(videoCards.Description[i]));
    }

    ui.timeComboBox->addItem(TIME_5M);
    ui.timeComboBox->addItem(TIME_15M);
    ui.timeComboBox->addItem(TIME_30M);
    ui.timeComboBox->addItem(TIME_1H);
    ui.timeComboBox->addItem(TIME_2H);
    ui.timeComboBox->addItem(TIME_4H);
    ui.timeComboBox->addItem(TIME_8H);
    ui.timeComboBox->addItem(TIME_16H);

    ui.tempLabel->setText("");
    ui.timeLabel->setText("");

}


GPUStressPage::~GPUStressPage()
{
}

void GPUStressPage::showEvent(QShowEvent* e)
{
    static bool s_intiDone = false;

    if (!s_intiDone)
    {
        ui.startTestButton->setText(GPU_TEST_START);
        m_gpuProcess.start(".\\GPUStress\\GPUStress.exe GETDEV");
        m_gpuProcess.waitForFinished();

        s_intiDone = true;
    }
}

void GPUStressPage::TestButtonClicked()
{
    if (ui.startTestButton->text() == GPU_TEST_START)
    {
        ui.deviceComboBox->setEnabled(false);
        ui.timeComboBox->setEnabled(false);
        ui.startTestButton->setText(GPU_TEST_STOP);

        int testTime = this->GetTestTime();
        QString devicePath = this->GetTestDevicePath();
        this->SetTestParam(testTime, devicePath);

        m_testStartTime = QDateTime::currentDateTime().toTime_t(); 
        m_gpuProcess.start(".\\GPUStress\\GPUStress.exe");
        m_gpuTimer.start();
    }
    else if (ui.startTestButton->text() == GPU_TEST_STOP)
    {
        m_gpuProcess.terminate();
    }
}

void GPUStressPage::GpuStressTimerTimeout()
{
    if (m_gpuProcess.state() == QProcess::NotRunning)
    {
        ui.deviceComboBox->setEnabled(true);
        ui.timeComboBox->setEnabled(true);
        ui.startTestButton->setText(GPU_TEST_START);

        m_gpuTimer.stop();
    }
    else if (m_gpuProcess.state() == QProcess::Running)
    {
        // 更新GPU温度
        unsigned int temp = 0;

        int currentIdx = ui.deviceComboBox->currentIndex();
        const VideoCardInforArray& videoCards = LHardwareInfor::GetVideoCardInfor();
        if (videoCards.Type[currentIdx]== VIDEO_CARD_INTERNAL)
        {
            CpuTempInfor cpuTemp;
            m_tempProbe.GetCpuTemp(cpuTemp);
            for (unsigned int i = 0; i < cpuTemp.CoreNum; i++)
            {
                temp += cpuTemp.CoreTemp[i];
            }
            temp /= cpuTemp.CoreNum;
        }
        else if (videoCards.Type[currentIdx]== VIDEO_CARD_EXTERNAL)
        {
            GpuTempInfor gpuTemp;
            m_tempProbe.GetGpuTemp(gpuTemp);
            for (unsigned int i = 0; i < gpuTemp.SensorsNum; i++)
            {
                temp += gpuTemp.Temp[i];
            }
            temp /= gpuTemp.SensorsNum;
        }

        ui.tempLabel->setText(QString::fromStdWString(L"%1 ℃").arg(temp));

        // 更新剩余时间
        unsigned int currentTime = QDateTime::currentDateTime().toTime_t();
        unsigned int time = (currentTime - m_testStartTime)/60;
        unsigned int remainTime = this->GetTestTime() - time;
        ui.timeLabel->setText(QString("%1 Min").arg(remainTime));
    }
    
}

int GPUStressPage::GetTestTime()
{
    QString strTime = ui.timeComboBox->currentText();
    if (strTime == TIME_5M)
        return 5;
    else if (strTime == TIME_15M)
        return 15;
    else if (strTime == TIME_30M)
        return 30;
    else if (strTime == TIME_1H)
        return 60;
    else if (strTime == TIME_2H)
        return 120;
    else if (strTime == TIME_4H)
        return 240;
    else if (strTime == TIME_8H)
        return 480;
    else if (strTime == TIME_16H)
        return 960;
    else
        return 10;
}

QString GPUStressPage::GetTestDevicePath()
{
    int currentIdx = ui.deviceComboBox->currentIndex();
    const VideoCardInforArray& videoCards = LHardwareInfor::GetVideoCardInfor();
    return QString::fromStdWString(videoCards.DevicePath[currentIdx]);
}

void GPUStressPage::SetTestParam(int testTime, const QString& devicePath)
{
    // Qt INI配置文件读写API和Windows INI配置文件读写API不兼容, 因为ExeParam.ini是由Win32程序使用Windows API创建的
    // 所以这里我们同样适用Windows API读写INI配置文件
    int deviceCount = GetPrivateProfileIntW(L"Device", L"Count", 0, GPU_TTEST_CONFIG_FILE);
    for (int i = 0; i < deviceCount; i++)
    {
        wchar_t strIdx[16] = {0};
        wsprintfW(strIdx, L"%d", i);
        wchar_t buffer[256] = { 0 };
        GetPrivateProfileStringW(strIdx, L"DevicePath", L"", buffer, 256, GPU_TTEST_CONFIG_FILE);

        QString path = QString::fromStdWString(buffer);
        if (path == devicePath)
        {
            WritePrivateProfileStringW(L"ExeParam", L"Device", strIdx, GPU_TTEST_CONFIG_FILE);
            break;
        }

    }

    wchar_t strTime[16] = {0};
    wsprintfW(strTime, L"%d", testTime);
    WritePrivateProfileStringW(L"ExeParam", L"Time", strTime, GPU_TTEST_CONFIG_FILE);

}

void GPUStressPage::LoadQSS(IN float uiRatio)
{
    ui.timeComboBox->setView(new QListView());
    ui.deviceComboBox->setView(new QListView());

    QString filePath = LAppParam::GetDefaultQssPath();
    filePath += "GPUStressPage.qss";
    QFile qssFile(filePath);     
    qssFile.open(QFile::ReadOnly);  

    if(qssFile.isOpen())  
    {  
        QString qss = qssFile.readAll();
        qssFile.close();  

        // 下拉框每项的高度需要动态设置, 不能在QSS文件中写死
        QString comboxItemHeightQss = QString::fromAscii(
            "QComboBox QAbstractItemView::item\
            {\
            color: black;\
            min-height: %1px;\
            }").arg((int)(25 * uiRatio));

        qss += comboxItemHeightQss;

        this->setStyleSheet(qss);
    } 
}