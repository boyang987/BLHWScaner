
#include "DiskSpeedPage.h"

#include <QtCore/QFile>
#include <QtGui/QListView>

#include "App\\AppParam.h"

#include "..\\Src\\Log\\LLog.h"
#include "..\\Src\\LHardwareInfor.h"
#include "..\\Src\\DiskController\\LDiskController.h"

#define DISK_TEST_START QString::fromStdWString(L"Test Start")
#define DISK_TEST_STOP QString::fromStdWString(L"Test Stop")
#define DISK_SPEED_ZERO QString::fromStdWString(L"0.00")

#define SEQ_TEST_FILENAME QString::fromStdWString(L"SquenceTest.temp");
#define RANDOM_4K_TEST_FILENAME QString::fromStdWString(L"Random4KTest.temp");

#define SIZE_32M  "32M"
#define SIZE_64M  "64M"
#define SIZE_128M "128M"
#define SIZE_256M "256M"
#define SIZE_512M "512M"
#define SIZE_1G   "1G"
#define SIZE_2G   "2G"
#define SIZE_4G   "4G"

DiskSpeedPage::DiskSpeedPage(IN float uiRatio, QWidget *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
    ui.setupUi(this);
    this->LoadQSS(uiRatio);

    m_pSeqTest = new LDiskSequenceTest();
    m_p4KRandTest = new LDisk4KRandomTest();

    QObject::connect(ui.pushButtonTest, SIGNAL(clicked()), this, SLOT(TestButtonClicked()));
    QObject::connect(ui.pushButtonUpdate, SIGNAL(clicked()), this, SLOT(UpdateButtonClicked()));

    m_seqTestTimer.setInterval(500);
    m_rand4KTestTimer.setInterval(500);

    QObject::connect(&m_seqTestTimer, SIGNAL(timeout()), this, SLOT(SeqTestMonitorTimer()));
    QObject::connect(&m_rand4KTestTimer, SIGNAL(timeout()), this, SLOT(Rand4KTestMonitorTimer()));

    ui.comboBoxSeqSize->clear();
    ui.comboBoxSeqSize->addItem(SIZE_32M);
    ui.comboBoxSeqSize->addItem(SIZE_64M);
    ui.comboBoxSeqSize->addItem(SIZE_128M);
    ui.comboBoxSeqSize->addItem(SIZE_256M);
    ui.comboBoxSeqSize->addItem(SIZE_512M);
    ui.comboBoxSeqSize->addItem(SIZE_1G);
    ui.comboBoxSeqSize->addItem(SIZE_2G);
    ui.comboBoxSeqSize->addItem(SIZE_4G);
    ui.comboBoxSeqSize->setCurrentIndex(6);

    ui.comboBoxRandSize->clear();
    ui.comboBoxRandSize->addItem(SIZE_32M);
    ui.comboBoxRandSize->addItem(SIZE_64M);
    ui.comboBoxRandSize->addItem(SIZE_128M);
    ui.comboBoxRandSize->addItem(SIZE_256M);
    ui.comboBoxRandSize->addItem(SIZE_512M);
    ui.comboBoxRandSize->addItem(SIZE_1G);
    ui.comboBoxRandSize->addItem(SIZE_2G);
    ui.comboBoxRandSize->addItem(SIZE_4G);
    ui.comboBoxRandSize->setCurrentIndex(5);

    // 如果在showEvent中更新磁盘信息, 那么第一次显示测速UI的时候有卡顿现象
    this->UpdateDiskInformation();
}

DiskSpeedPage::~DiskSpeedPage()
{
    if (m_pSeqTest != 0)
    {
        delete m_pSeqTest;
        m_pSeqTest = 0;
    }

    if (m_p4KRandTest != 0)
    {
        delete m_p4KRandTest;
        m_p4KRandTest = 0;
    }
}

void DiskSpeedPage::showEvent(QShowEvent* e)
{
    static bool s_intiDone = false;

    if (!s_intiDone)
    {
        ui.labelSeqWSpeed->setText(DISK_SPEED_ZERO);
        ui.labelSeqRSpeed->setText(DISK_SPEED_ZERO);
        ui.label4KRandWSpeed->setText(DISK_SPEED_ZERO);
        ui.label4KRandRSpeed->setText(DISK_SPEED_ZERO);

        ui.labelTestState->setText("");

        ui.pushButtonTest->setText(DISK_TEST_START);

        s_intiDone = true;
    }
    
}

void DiskSpeedPage::TestButtonClicked()
{
    if (ui.pushButtonTest->text() == DISK_TEST_START)
    {
        int diskIndex = ui.comboBoxDiskList->currentIndex();
        this->GetTestFileSize();

        unsigned long long driveFreeSpace = 0;
        this->SelectLogicalDrive(m_diskLogicalNameList[diskIndex], &m_currentTestLogicalDrive, &driveFreeSpace);
        PrintLogW(L"");
        PrintLogW(L"Current Test Logical Drive: %s, Free Space: %I64u", 
            m_currentTestLogicalDrive.toStdWString().c_str(),
            driveFreeSpace);
        if (driveFreeSpace < (unsigned long long)(m_randSize) * 1024 * 1024 ||
            driveFreeSpace < (unsigned long long)(m_seqSize) * 1024 * 1024)
        {
            ui.labelTestState->setText(QString::fromStdWString(L"Disk Free Size Is Tool Small"));
            PrintLogW(L"Disk Free Size Is Tool Small");
            return;
        }

        ui.labelSeqWSpeed->setText(DISK_SPEED_ZERO);
        ui.labelSeqRSpeed->setText(DISK_SPEED_ZERO);
        ui.label4KRandWSpeed->setText(DISK_SPEED_ZERO);
        ui.label4KRandRSpeed->setText(DISK_SPEED_ZERO);
        ui.pushButtonTest->setText(DISK_TEST_STOP);
        ui.labelTestState->setText("Sequence Test Is Running...");
        ui.comboBoxDiskList->setEnabled(false);
        ui.pushButtonUpdate->setEnabled(false);
        ui.comboBoxSeqSize->setEnabled(false);
        ui.comboBoxRandSize->setEnabled(false);
  
        PrintLogW(L"Start Disk Speed Test");
        
        QString testFilePath = m_currentTestLogicalDrive;
        testFilePath += QString::fromStdWString(L"\\");
        testFilePath += SEQ_TEST_FILENAME;
        PrintLogW(L"Sequence Test File Path: %s", testFilePath.toStdWString().c_str());

        m_pSeqTest->Start(testFilePath.toStdWString(), m_seqSize);
        m_seqTestTimer.start();

        PrintLogW(L"Start Disk Sequence Test");

    }
    else if (ui.pushButtonTest->text() == DISK_TEST_STOP)
    {
        ui.pushButtonTest->setText(DISK_TEST_START);
        ui.labelTestState->setText("");
        ui.comboBoxDiskList->setEnabled(true);
        ui.pushButtonUpdate->setEnabled(true);
        ui.comboBoxSeqSize->setEnabled(true);
        ui.comboBoxRandSize->setEnabled(true);

        PrintLogW(L"User Stop Disk Speed Test");

        m_pSeqTest->Stop();
        m_p4KRandTest->Stop();

        m_seqTestTimer.stop();
        m_rand4KTestTimer.stop();

        
    }
}

void DiskSpeedPage::UpdateButtonClicked()
{
    ui.comboBoxDiskList->setEnabled(false);
    ui.pushButtonTest->setEnabled(false);

    this->UpdateDiskInformation();

    ui.comboBoxDiskList->setEnabled(true);
    ui.pushButtonTest->setEnabled(true);
}

void DiskSpeedPage::SeqTestMonitorTimer()
{
    LDiskSpeedTestState state = m_pSeqTest->GetState();
    QString readSpeed;
    QString writeSpeed;
    if (state.ReadSpeed >= 1000.0f)
        readSpeed = QString::number(state.ReadSpeed, 'f', 1);
    else
        readSpeed = QString::number(state.ReadSpeed, 'f', 2);

    if (state.WriteSpeed >= 1000.0f)
        writeSpeed = QString::number(state.WriteSpeed, 'f', 1);
    else
        writeSpeed = QString::number(state.WriteSpeed, 'f', 2);

    ui.labelSeqRSpeed->setText(readSpeed);
    ui.labelSeqWSpeed->setText(writeSpeed);

    // 测试未结束
    if (!state.TestDone)
        return;

    // 测试结束
    m_seqTestTimer.stop();

    // 测试没发生错误则进行4K随机测试
    if (state.Error == DST_NO_ERROR)
    {
        ui.labelTestState->setText("4K Random Test Is Running...");

        PrintLogW(L"Disk Sequence Test Is Done, Read Speed: %s MB/s, Write Speed: %s MB/s", 
            readSpeed.toStdWString().c_str(), 
            writeSpeed.toStdWString().c_str());

        QString testFilePath = m_currentTestLogicalDrive;
        testFilePath += QString::fromStdWString(L"\\");
        testFilePath += RANDOM_4K_TEST_FILENAME;
        PrintLogW(L"Disk 4K Random Test File Path: %s", testFilePath.toStdWString().c_str());

        m_p4KRandTest->Start(testFilePath.toStdWString(), m_randSize);
        m_rand4KTestTimer.start();
        
        PrintLogW(L"Start Disk 4K Random Test");
    }

    // 测试发生错误, 显示错误消息, 停止测试
    if (state.Error != DST_NO_ERROR)
    {
        ui.labelTestState->setText(QString::fromStdWString(state.ErrorMsg));
        ui.pushButtonTest->setText(DISK_TEST_START);
        ui.comboBoxDiskList->setEnabled(true);
        ui.pushButtonUpdate->setEnabled(true);
        ui.comboBoxSeqSize->setEnabled(true);
        ui.comboBoxRandSize->setEnabled(true);

        PrintLogW(L"Disk Sequence Test Error, Message: %s", state.ErrorMsg.c_str());
        PrintLogW(L"Disk Sequence Test Error, Detailed Message: %s", state.ErrorMsgWindows.c_str());
    }
    
}

void DiskSpeedPage::Rand4KTestMonitorTimer()
{
    LDiskSpeedTestState state = m_p4KRandTest->GetState();

    QString readSpeed;
    QString writeSpeed;
    if (state.ReadSpeed >= 1000.0f)
        readSpeed = QString::number(state.ReadSpeed, 'f', 1);
    else
        readSpeed = QString::number(state.ReadSpeed, 'f', 2);

    if (state.WriteSpeed >= 1000.0f)
        writeSpeed = QString::number(state.WriteSpeed, 'f', 1);
    else
        writeSpeed = QString::number(state.WriteSpeed, 'f', 2);

    ui.label4KRandRSpeed->setText(readSpeed);
    ui.label4KRandWSpeed->setText(writeSpeed);

    if (!state.TestDone)
        return;

    m_rand4KTestTimer.stop();

    ui.pushButtonTest->setText(DISK_TEST_START);
    ui.comboBoxDiskList->setEnabled(true);
    ui.pushButtonUpdate->setEnabled(true);
    ui.comboBoxSeqSize->setEnabled(true);
    ui.comboBoxRandSize->setEnabled(true);

    if (state.Error == DST_NO_ERROR)
    {
        PrintLogW(L"Disk 4K Random Test Is Done, Read Speed: %s MB/s, Write Speed: %s MB/s", 
            readSpeed.toStdWString().c_str(), 
            writeSpeed.toStdWString().c_str());

        ui.labelTestState->setText("");
    }
    else
    {
        PrintLogW(L"Disk 4K Random Test Error, Message: %s", state.ErrorMsg.c_str());
        PrintLogW(L"Disk 4K Random Test Error, Detailed Message: %s", state.ErrorMsgWindows.c_str());

        ui.labelTestState->setText(QString::fromStdWString(state.ErrorMsg));
    }
}

void DiskSpeedPage::UpdateDiskInformation()
{
    m_diskLogicalNameList.clear();
    QVector<QString> diskModelNameList;

    const DiskInforArray& diskInforArray = LHardwareInfor::GetDiskInfor();
    for (unsigned int i = 0; i < diskInforArray.Count; i++)
    {
        DISK_TYPE diskType = diskInforArray.DiskType[i];
        QString modelName;
        if (FIXED_DISK == diskType)
        {
            switch (diskInforArray.FixedDiskType[i])
            {
            case FIXED_DISK_HDD:
                modelName += "(HDD)";
                break;
            case FIXED_DISK_SATA_SSD:
                modelName += "(SATA SSD)";
                break;
            case FIXED_DISK_EMMC:
                modelName += "(EMMC)";
                break;
            case FIXED_DISK_RAID:
                modelName += "(RAID)";
                break;
            case FIXED_DISK_NVME_SSD:
                modelName += "(NVME SSD)";
                break;
            default:
                modelName += "(Unknown)";
                break;
            }
        }

        if (EXTERNAL_USB_DISK == diskType)
        {
            modelName += "(Exteral USB)";
        }

        if (REMOVABLE_DISK == diskType)
        {
            modelName += "(Removable)";
        }

        if (VIRTUAL_DISK == diskType)
        {
            modelName += "(Virtual)";
        }

        if (UNKNOWN_DISK == diskType)
        {
            modelName += "(Unknown)";
        }
        
   

        modelName += QString::fromStdWString(diskInforArray.Model[i]);

        if (!diskInforArray.LogicalName[i].empty())
        {
            m_diskLogicalNameList.push_back(QString::fromStdWString(diskInforArray.LogicalName[i]));
            diskModelNameList.push_back(modelName);
        }
    }

    ui.comboBoxDiskList->clear();
    for (int i = 0; i < diskModelNameList.size(); i++)
    {
        QString showName;
        showName += QString::fromStdWString(L"(");
        showName += m_diskLogicalNameList[i];
        showName += QString::fromStdWString(L")");
        showName.replace(QString::fromStdWString(L";"), QString::fromStdWString(L" "));
        showName += diskModelNameList[i];

        ui.comboBoxDiskList->addItem(showName);

    }

    if (diskModelNameList.size() != 0)
    {
        ui.comboBoxDiskList->setCurrentIndex(0);
    }
}

bool DiskSpeedPage::SelectLogicalDrive(
    IN const QString& diskLogicalName, 
    OUT QString* pDrive, 
    OUT unsigned long long* pDriveFreeSpace)
{
    if (diskLogicalName.isEmpty())
        return false;

    QStringList driveList = diskLogicalName.split(QString::fromStdWString(L";"));
    if (driveList.size() < 1)
        return false;

    QString driveName;
    unsigned long long freeSpaceMax = 0;
    for (int i = 0; i < driveList.size(); i++)
    {
        unsigned long long freeSpace = 0;
        GetLogicalDriveFreeSpace(driveList[i].toStdWString(), &freeSpace);
        if (freeSpace > freeSpaceMax)
        {
            freeSpaceMax = freeSpace;
            driveName = driveList[i];
        }
    }

    if (pDrive != 0)
    {
        (*pDrive) = driveName;
    }

    if (pDriveFreeSpace != 0)
    {
        (*pDriveFreeSpace) = freeSpaceMax;
    }

    return true;
}

void DiskSpeedPage::GetTestFileSize()
{
    m_seqSize = 0;
    m_randSize = 0;

    QString seqSizeStr = ui.comboBoxSeqSize->currentText();
    if (SIZE_32M == seqSizeStr)
    {
        m_seqSize = 32;
    }
    if (SIZE_64M == seqSizeStr)
    {
        m_seqSize = 64;
    }
    if (SIZE_128M == seqSizeStr)
    {
        m_seqSize = 128;
    }
    if (SIZE_256M == seqSizeStr)
    {
        m_seqSize = 256;
    }
    if (SIZE_512M == seqSizeStr)
    {
        m_seqSize = 512;
    }
    if (SIZE_1G == seqSizeStr)
    {
        m_seqSize = 1024;
    }
    if (SIZE_2G == seqSizeStr)
    {
        m_seqSize = 2048;
    }
    if (SIZE_4G == seqSizeStr)
    {
        m_seqSize = 4095;
    }

    QString randSizeStr = ui.comboBoxRandSize->currentText();
    if (SIZE_32M == randSizeStr)
    {
        m_randSize = 32;
    }
    if (SIZE_64M == randSizeStr)
    {
        m_randSize = 64;
    }
    if (SIZE_128M == randSizeStr)
    {
        m_randSize = 128;
    }
    if (SIZE_256M == randSizeStr)
    {
        m_randSize = 256;
    }
    if (SIZE_512M == randSizeStr)
    {
        m_randSize = 512;
    }
    if (SIZE_1G == randSizeStr)
    {
        m_randSize = 1024;
    }
    if (SIZE_2G == randSizeStr)
    {
        m_randSize = 2048;
    }
    if (SIZE_4G == randSizeStr)
    {
        m_randSize = 4095;
    }
}

void DiskSpeedPage::LoadQSS(IN float uiRatio)
{
    ui.comboBoxDiskList->setView(new QListView());
    ui.comboBoxSeqSize->setView(new QListView());
    ui.comboBoxRandSize->setView(new QListView());

    QString filePath = LAppParam::GetDefaultQssPath();
    filePath += "DiskSpeedPage.qss";
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