



#ifndef _RESTARTAGING_PAGE_H_
#define _RESTARTAGING_PAGE_H_

#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QTimer>

#include "ui_RestartAging.h"

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 重复开机测试参数
struct RestartAgingParam
{
    int IntervalTime; ///< 间隔时间
    int AgingTimes; ///< 测试次数
    int CompletedTimes; ///< 已完成测试次数
    int BSODTimes; ///< 发生蓝屏的次数
    QString LastRestartTime; ///< 最新重开机时间, 格式"yyyy-MM-dd hh:mm:ss"
};

/// @brief 重复开机测试页面
class RestartAgingPage : public QDialog
{
    Q_OBJECT

public:
    /// @brief 构造函数
    RestartAgingPage(IN QWidget *parent = 0, IN Qt::WFlags flags = 0);

    /// @brief 析构函数
    ~RestartAgingPage();

protected:
    /// @brief 窗口显示事件
    void showEvent(QShowEvent* e);

private slots:
    void TestButtonClicked();
    void ResetButtonClicked();

    /// @brief 倒计时定时器超时回调函数
    void CountDownTimerTimeout();

private:
    /// @brief 注册任务计划
    void RegisterTaskScheduler();

    /// @brief 删除任务计划
    void DeleteTaskScheduler();

    /// @brief 保存测试参数到文件中
    void SaveTestParamToFile(IN const RestartAgingParam& param);

    /// @brief 从文件中获取测试参数
    void GetTestParamFromFile(OUT RestartAgingParam& param);

    /// @brief 判断是否发生蓝屏
    /// @param[in] lastRestartTime 上次关机时间
    /// @return 发生蓝屏返回true, 未发生蓝屏返回false
    bool IsBSODHappened(IN const QString& lastRestartTime);

    /// @brief 检查路径, 路径不存在则创建
    /// @param[in] qstrPath 路径
    /// @return 成功返回true, 失败返回false
    bool CheckPath(IN const QString& qstrPath);

    /// @brief 加载UI样式
    void LoadQSS();

private:
    Ui::RestartAgingForm ui;
    int m_currentCountDown; ///< 当前倒计时
    QTimer m_countDownTimer; ///< 倒计时定时器
};

#endif