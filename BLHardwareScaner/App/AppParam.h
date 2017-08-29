

#ifndef _APP_H_
#define _APP_H_

#include <QtCore/QString>

/// @brief 程序启动模式
enum APP_START_MODE
{
    APP_NORMAL = 0, ///< 标准启动模式
    APP_RESTARTAGING ///< 重开机Aging启动模式
};

/// @brief 程序参数类
class LAppParam
{
public:
    /// @brief 获取程序启动模式
    /// @return 程序启动模式
    static APP_START_MODE GetStartMode();

    /// @brief 获取程序当前版本
    /// @return 版本信息, 如V1.3.6
    static QString GetAppVersion();

    /// @brief 获取默认QSS文件路径
    /// @return QSS文件路径, 如".\\Config\\QSS\\Default\\"
    static QString GetDefaultQssPath();

private:
    /// @brief 初始化程序参数
    /// @param[in] argc 命令行参数数目
    /// @param[in] argv[] 命令行参数
    static void InitAppParam(int argc, char *argv[]);
    friend int main(int argc, char *argv[]); ///< 设置main函数为友元
};

#endif