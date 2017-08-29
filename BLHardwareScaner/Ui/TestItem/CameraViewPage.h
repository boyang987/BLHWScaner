

#ifndef _CAMERAVIRW_PAGE_H_
#define _CAMERAVIEW_PAGE_H_

#include <QtGui/QDialog>
#include <QtCore/QVector>

#include "ui_CameraView.h"

#include "..\\Src\\LHardwareInfor.h"
#include "..\\Src\\Webcam\\LWebcam.h"

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 摄像头预览页面
class CameraViewPage : public QDialog
{
    Q_OBJECT

public:
    /// @brief 构造函数
    CameraViewPage(IN QWidget *parent = 0, IN Qt::WFlags flags = 0);

    /// @brief 析构函数
    ~CameraViewPage();

protected:
    /// @brief 窗口显示事件
    void showEvent(QShowEvent* e);

    /// @brief 窗口隐藏事件
    void hideEvent(QHideEvent* e);

private slots:
   /// @brief 转换按钮单击槽函数
   void SwitchButtonClicked();

private:
   /// @brief 加载UI样式
   void LoadQSS();

private:
    /// @brief 显示当前摄像头
    void ShowCurrentCamera();

    /// @brief 关闭当前摄像头
    void CloseCurrentCamera();

    /// @brief 获取Webcam接口指针
    /// @param[in] index Webcam索引
    LWebcam* GetWebcam(IN unsigned int index);


private:
    Ui::CameraViewForm ui;

    int m_currentCameraIndex; ///< 当前显示的摄像头索引
    unsigned int m_cameraTotalCount; ///< 摄像头总数
    LWebcam* m_pCurrentWebcam; ///< 当前显示的摄像头
};

#endif