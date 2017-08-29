

#ifndef _HARDWAREINFORPAGE_H_
#define _HARDWAREINFORPAGE_H_

#include <QtCore/QVector>
#include <QtGui/QSplashScreen>

#include "ui_HardwareInfor.h"

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

class HWItemInfor;

/// @brief 硬件信息页面
class HardwareInforPage : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param[in] uiRatio UI放大系数
    HardwareInforPage(IN float uiRatio, IN QWidget *parent = 0, IN Qt::WFlags flags = 0);
    ~HardwareInforPage();

public:
    /// @brief 设置启动画面指针
    /// @param[in] pSplashScrren 启动画面指针
    void SetSplashScreen(IN QSplashScreen* pSplashScrren);

    /// @brief 初始化硬件信息
    void InitHardwareInfor();

protected:
    void showEvent(QShowEvent* e);

private slots:
    /// @brief 当前所选项改变槽函数
    void CurrentItemChanged();

private:
    /// @brief 加载UI样式
    /// @param[in] uiRatio UI放大系数
    void LoadQSS(IN float uiRatio);

    /// @brief 启动画面显示消息
    /// @param[in] msg 消息
    void SplashScreenShow(IN const QString& msg);

    /// @brief 增加一个提示消息
    /// @param[in] msg 消息
    void AddTips(IN const QString& msg);

    /// @brief 收集提示信息
    void CollectTips();

private:
    Ui::HardwareInforForm ui;
    QSplashScreen* m_pSplashScrreen; ///< 启动画面

    QVector<HWItemInfor*> m_hwItemVec; ///< 硬件项列表
    QVector<QLabel*> m_tipsLabelVec; ///< 提示标签列表
};

/// @brief 硬件项信息
class HWItemInfor
{
public:
    HWItemInfor();
    virtual ~HWItemInfor();

    /// @brief 获取项标题
    /// @return 项标题
    const QString& GetTitle() const;

    /// @brief 获取项内容
    /// @return 项内容
    const QString& GetContent() const;


    /// @brief 加载硬件信息
    virtual void LoadHWInfor() = 0;

protected:
    /// @brief 设置项标题
    ///  
    /// @param[in] title 标题
    void SetTitle(IN const QString& title);

    /// @brief 内容中增加一项
    /// @param[in] subTitle 内容中的子标题
    /// @param[in] text 文本
    void ContentAddItem(IN const QString& subTitle, IN const QString text);

    /// @brief 内容中增加空行
    void ContentAddBlankLine();

    /// @brief 清除现有的信息
    void ClearInfor();

private:
    QString m_title; ///< 项标题
    QString m_content; ///< 项类容
};

/// @brief 计算机项信息项信息
class ComputerItemInfor : public HWItemInfor
{
public: 

    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 主板项信息
class MotherBoardItemInfor : public HWItemInfor
{
public:

    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 处理器项信息
class ProcessorItemInfor : public HWItemInfor
{
public:

    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};


/// @brief 显卡项信息
class VideoCardItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 内存项信息
class MemoryItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 磁盘项信息
class DiskItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 网卡项信息
class NetworkCardItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 显示器项信息
class MonitorItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 电池项信息
class BatteryItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 摄像机项信息
class CameraItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief CDRom驱动器项信息
class CDRomDriveItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();
};

/// @brief 传感器项信息
class SensorsItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();

};

/// @brief 生物识别设备项信息
class BiometricItemInfor : public HWItemInfor
{
public:
    /// @brief 加载硬件信息
    virtual void LoadHWInfor();

};

#endif