/// @file LMemory.h
/// @brief 该文件中声明了内存操作类 
/// 
/// Detail:
/// @author Burnell_Liu  
/// @version   
/// @date 9:11:2015

#ifndef _LMEMORY_H_
#define _LMEMORY_H_


    /// @brief 内存类
class LMemory
{
public:
    LMemory();
    ~LMemory();

    /// @brief 获取内存使用百分比
    /// @return 成功返回百分比, 失败返回-1
    int GetLoadPercent();

    /// @brief 获取总内存大小(单位M)
    /// @return 成功返回内存大小, 失败返回-1
    int GetTotalSize();

    /// @brief 获取可用内存大小
    /// @return 成功返回可用内存大小, 失败返回-1
    int GetAvailableSize();
};

#endif