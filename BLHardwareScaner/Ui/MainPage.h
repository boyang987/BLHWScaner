#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSplashScreen>
#include <QtGui/QMouseEvent>
#include <QtCore/QTimer>

#include "ui_Main.h"

#include "Update/NewVersionUpdate.h"

class HardwareInforPage;
class TempManagementPage;
class TestItemPage;

class MainPage : public QMainWindow
{
    Q_OBJECT

public:
    MainPage(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MainPage();

protected:
    /// @brief 窗口显示事件
    void showEvent(QShowEvent* e);

    /*
    去掉系统边框和标题栏后导致程序窗口无法移动
    手动处理以下三个事件来实现窗口移动
    */
    /// @brief 鼠标按下事件
    void mousePressEvent(QMouseEvent *e);

    /// @brief 鼠标移动事件, 只有鼠标处于按下状态时才会触发该事件
    void mouseMoveEvent(QMouseEvent *e);

    /// @brief 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void HardwareInforClicked();
    void TemperatureClicked();
    void TestItemClicked();

    void MinButtonClicked();
    void CloseButtonClicked();
    void UpdateButtonClicked();

    /// @brief 检查新版本计时器超时回调函数
    void CheckNewTimerTimeout();

    /// @brief 下载新版本计时器超时回调函数
    void DownloadNewTimerTimeout();

private:
    /// @brief 加载UI样式
    void LoadQSS();

private:
    Ui::MainForm ui;
    QSplashScreen m_splashScreen; ///< 启动画面

    bool m_mousePressed; ///< 标志鼠标是否被按下
    QPoint m_mouseLastPos; ///< 最新鼠标位置

    HardwareInforPage* m_pHardwareInforPage;
    TempManagementPage* m_pTempManagementPage;
    TestItemPage* m_pTestItemPage;

    float m_uiRatio; ///< UI需要调整的比例

    CheckNew m_checkNew; ///< 检查新版本
    QTimer m_checkNewTimer; ///< 检查新版本定时器

    DownloadNew m_downloadNew; ///< 下载新版本
    QTimer m_downloadNewTimer; ///< 下载新版本定时器
};

#endif // MAINWINDOW_H
