

#ifndef _DISKSPEED_PAGE_H_
#define _DISKSPEED_PAGE_H_

#include <QtGui/QDialog>
#include <QtCore/QVector>
#include <QtCore/QTimer>

#include "ui_DiskSpeed.h"
#include "..\\Src\\LDiskSpeed.h"

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 磁盘速度测试页面
class DiskSpeedPage : public QDialog
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param[in] uiRatio UI放大系数
    DiskSpeedPage(IN float uiRatio, IN QWidget *parent = 0, IN Qt::WFlags flags = 0);

    /// @brief 析构函数
    ~DiskSpeedPage();

protected:
    /// @brief 窗口显示事件
    void showEvent(QShowEvent* e);

private slots:
    /// @brief 测试按钮单击槽函数
    void TestButtonClicked();

    /// @brief 更新按钮单击槽函数
    void UpdateButtonClicked();

    /// @brief 顺序测试监测计时器超时回调函数
    void SeqTestMonitorTimer();

    /// @brief 4K随机测试监测计时器超时回调函数
    void Rand4KTestMonitorTimer();

private:
    /// @brief 更新磁盘信息
    void UpdateDiskInformation();

    /// @brief 从磁盘的逻辑分区中选择一个
    /// 优先选择剩余空间大的分区
    /// @param[in] diskLogicalName 磁盘逻辑名称, 多个分区以分号隔开如: C:;D:;E:
    /// @param[out] pDrive 存储选择的逻辑分区
    /// @param[out] pDriveFreeSpace 逻辑分区的剩余空间, 单位字节
    /// @return 成功返回true, 失败返回false
    bool SelectLogicalDrive(
        IN const QString& diskLogicalName, 
        OUT QString* pDrive, 
        OUT unsigned long long* pDriveFreeSpace);

    /// @brief 获取测试文件大小
    void GetTestFileSize();

    /// @brief 加载UI样式
    /// @param[in] uiRatio UI放大系数
    void LoadQSS(IN float uiRatio);

private:
    Ui::DiskSpeedForm ui;

    unsigned long m_seqSize; ///< 顺序测试文件大小, 单位M
    unsigned long m_randSize; ///< 随机测试文件大小, 单位M

    QTimer m_seqTestTimer; ///< 顺序测试监测定时器
    QTimer m_rand4KTestTimer; ///< 4K随机测试监测定时器

    IDiskSpeedTest* m_pSeqTest; ///< 顺序测试
    IDiskSpeedTest* m_p4KRandTest; ///< 4K随机测试

    QString m_currentTestLogicalDrive; ///< 当前需要测试的逻辑分区
    QVector<QString> m_diskLogicalNameList; ///< 磁盘逻辑盘符名称列表

};

#endif