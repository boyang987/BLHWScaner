



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
	QString Action; ///< 动作
    int IntervalTime; ///< 间隔时间
    int AgingTimes; ///< 测试次数
    int CompletedTimes; ///< 已完成测试次数
    int BSODTimes; ///< 发生蓝屏的次数
    int DeviceLose; ///< 设备丢失的次数
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
	/// @brief 测试按钮点击回调函数
    void TestButtonClicked();

	/// @brief 重置按钮点击回调函数
    void ResetButtonClicked();

    /// @brief 选择项点击回调函数
    void CheckBoxClicked();

    /// @brief 倒计时定时器超时回调函数
    void CountDownTimerTimeout();

	/// @brief 系统从挂起中恢复时的回调函数
	void ResumeSuspend();

private:

    /// @brief 设置界面处于测试状态
    void SetUITesting();

    /// @brief 设置界面不处于测试状态
    void SetUINoTesting();

    /// @brief 设置界面上的测试参数
    void SetUITestParam(const RestartAgingParam& param);

	/// @brief 判断S3或S4测试是否正在进行
	bool IsS3S4Running();

    /// @brief 注册重启任务计划
    void RegisterRestartTaskScheduler();

    /// @brief 删除重启任务计划
    void DeleteRestartTaskScheduler();

    /// @brief 保存测试参数到文件中
    void SaveTestParamToFile(IN const RestartAgingParam& param);

    /// @brief 从文件中获取测试参数
    void GetTestParamFromFile(OUT RestartAgingParam& param);

    /// @brief 获取当前设备
    void GetCurrentDevices(OUT QSet<QString>& devicesList);

    /// @brief 保存设备信息到文件中
    void SaveDevicesToFile(IN const QSet<QString>& devicesList);

    /// @brief 从文件中获取设备信息
    void GetDevicesFromFile(OUT QSet<QString>& devicesList);

    /// @brief 将丢失是被保存在文件中
    void SaveLoseDevicesToFile(IN const QList<QString>& loseDevices);

    /// @brief 删除丢失设备文件
    void DeleteLoseDevicesFile();

    /// @brief 判断是否发生蓝屏
    /// @param[in] lastRestartTime 上次关机时间
    /// @return 发生蓝屏返回true, 未发生蓝屏返回false
    bool IsBSODHappened(IN const QString& lastRestartTime);

    /// @brief 判断是否发生设备丢失
    /// @return 发生设备丢失返回true, 未发生设备丢失返回false
    bool IsDeviceLoseHappened(OUT QList<QString>& loseDevices);

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