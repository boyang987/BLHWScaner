

#ifndef _LAPPINSTANCE_H_
#define _LAPPINSTANCE_H_

#include <string>
using std::wstring;


#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif


class CAppInstance;

/// @brief 应用程序实例类
/// 该类用于检测应用程序是否已经存在一个实例在运行, 通过实例名称来识别
/// 请保证该类对象的作用域, 作用在整个应用程序的生命周期
class LAppInstance
{
public:
    /// @brief 构造函数
    /// @param[in] name 实例名称
    explicit LAppInstance(IN const wstring& name);

    /// @brief 析构函数
    ~LAppInstance();

    /// @brief 判断是否已经存在该实例 
    /// @return true(已经存在), false(新实例)
    bool IsAlreadyExist();

private:
    CAppInstance* m_pAppInstance; ///< 应用程序实例实现对象
};


#endif