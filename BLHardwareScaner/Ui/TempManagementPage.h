
#ifndef _TEMPMANAGEMENTPAGE_H_
#define _TEMPMANAGEMENTPAGE_H_

#include "ui_TempManagement.h"

#include <QVector>
#include <QTimer>
#include <QThread>

#include "..\\Src\\LCpuStress.h"

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 扫描温度线程类
class ScanTempThread : public QThread
{
    Q_OBJECT
public:
    ScanTempThread();
    ~ScanTempThread();

    /// @brief 停止线程
    void Stop();

protected:
    virtual void run();

private:
    bool m_bStopThread; ///< 标记是否停止线程

};

/// @brief 扫描性能线程类
class ScanPerformanceThread : public QThread
{
    Q_OBJECT
public:
    ScanPerformanceThread();
    ~ScanPerformanceThread();

    /// @brief 停止线程
    void Stop();

protected:
    virtual void run();

private:
    bool m_bStopThread; ///< 标记是否停止线程

};


class TempManagementPage : public QWidget
{
    Q_OBJECT

public:
    TempManagementPage(IN QWidget *parent = 0, IN Qt::WFlags flags = 0);
    ~TempManagementPage();

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    /// @brief UI刷新计算器超时回调函数
    void UiRefreshTimerTimeout();

    /// @brief CPU动作按钮槽函数
    void CpuActionButtonClicked();

    /// @brief CPU压力测试定时器超时函数
    void CpuStressTimerTimeout();

private:
    /// @brief 刷新UI
    void RefreshUi();

    /// @brief 加载UI样式
    void LoadQSS();

private:
    Ui::TempManagementForm ui;
    QTimer* m_pUiRefreshTimer; ///< 刷新UI定时器

    ScanTempThread m_scanTempThread; ///< 扫描温度线程
    ScanPerformanceThread m_scanPerformanceThred; ///< 扫描性你呢线程

    QTimer* m_pCpuStressTimer; ///< CPU压力测试定时器
    LCpuStressTest m_cpuStressTest; ///< CPU压力测试
    
    bool m_bExternalVideoCardExist; ///< 标识是否存在独立显卡
};





#endif