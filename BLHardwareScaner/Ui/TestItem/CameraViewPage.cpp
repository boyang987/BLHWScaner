
#include "CameraViewPage.h"

#include <QtCore/QFile>

#include "App\\AppParam.h"




CameraViewPage::CameraViewPage(IN QWidget *parent /* = 0 */, IN Qt::WFlags flags /* = 0 */)
    : QDialog(parent, flags)
{
    ui.setupUi(this);
    this->LoadQSS();

    QObject::connect(ui.pushButtonSwitch, SIGNAL(clicked()), this, SLOT(SwitchButtonClicked()));

    m_currentCameraIndex = -1;
    m_pCurrentWebcam = 0;

    const CameraInforArray& cameraInfor = LHardwareInfor::GetCameraInfor();
    m_cameraTotalCount = cameraInfor.Count;

    ui.pushButtonSwitch->setEnabled(false);
    ui.pushButtonSwitch->setVisible(false);

    ui.labelCameraName->setText("");

    if (m_cameraTotalCount > 1)
    {
        ui.pushButtonSwitch->setEnabled(true);
        ui.pushButtonSwitch->setVisible(true);
    }

    if (m_cameraTotalCount > 0)
    {
        m_currentCameraIndex = 0;
        m_pCurrentWebcam = this->GetWebcam(m_currentCameraIndex);
    }
}

CameraViewPage::~CameraViewPage()
{

}

void CameraViewPage::showEvent(QShowEvent* e)
{
    this->ShowCurrentCamera();

}

void CameraViewPage::hideEvent(QHideEvent* e)
{
    this->CloseCurrentCamera();
}

void CameraViewPage::SwitchButtonClicked()
{
    
    this->CloseCurrentCamera();
    m_pCurrentWebcam->Release();
    m_pCurrentWebcam = 0;

    m_currentCameraIndex++;
    if (m_currentCameraIndex == m_cameraTotalCount)
    {
        m_currentCameraIndex = 0;
    }

    m_pCurrentWebcam = this->GetWebcam(m_currentCameraIndex);

    this->ShowCurrentCamera();
    
}

void CameraViewPage::LoadQSS()
{
    QString filePath = LAppParam::GetDefaultQssPath();
    filePath += "CameraViewPage.qss";
    QFile qssFile(filePath);   
  
    qssFile.open(QFile::ReadOnly);  

    if(qssFile.isOpen())  
    {  
        QString qss = qssFile.readAll();
        qssFile.close();  

        this->setStyleSheet(qss);
    } 
}

void CameraViewPage::ShowCurrentCamera()
{
    if (m_pCurrentWebcam == 0)
        return;

    wstring wStrName;
    m_pCurrentWebcam->GetFriendlyName(wStrName);
    QString qStrName = QString::fromStdWString(wStrName);
    ui.labelCameraName->setText(qStrName);

    WId wid = ui.widgetCamera->winId();
    m_pCurrentWebcam->SetParentWindow(wid, 0, 0, ui.widgetCamera->width(), ui.widgetCamera->height());
    m_pCurrentWebcam->Run();
}

void CameraViewPage::CloseCurrentCamera()
{
    if (m_pCurrentWebcam == 0)
        return;

    m_pCurrentWebcam->Stop();
}

LWebcam* CameraViewPage::GetWebcam(IN unsigned int index)
{
    QVector<LWebcam*> webcamQVec;

    const CameraInforArray& cameraInfor = LHardwareInfor::GetCameraInfor();
    // 枚举出真实摄像头
    vector<LWebcam*> webcamList;
    string error;
    LEnumWebcam(webcamList, error);
    for (unsigned int i = 0; i < webcamList.size(); i++)
    {
        wstring friendlyName;
        webcamList[i]->GetFriendlyName(friendlyName);
        for (unsigned long j = 0; j < cameraInfor.Count; j++)
        {
            if (friendlyName.compare(cameraInfor.Name[j]) == 0)
            {
                webcamQVec.push_back(webcamList[i]);
                webcamList[i] = 0;
            }
        }
    }

    for (unsigned int i = 0; i < webcamList.size(); i++)
    {
        if (webcamList[i] != 0)
        {
            webcamList[i]->Release();
            webcamList[i] = 0;
        }
    }

    if (index >= webcamQVec.size())
        return 0;

    for (int i = 0; i < webcamQVec.size(); i++)
    {
        if (index == i)
            continue;

        if (webcamQVec[i] != 0)
        {
            webcamQVec[i]->Release();
            webcamQVec[i] = 0;
        }

    }

    return webcamQVec[index];
}