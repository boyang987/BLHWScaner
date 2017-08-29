

#ifndef _LPDH_H_
#define _LPDH_H_

#include <string>
using std::wstring;


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

class CPdh;

/// @brief PDH接口类
class LPdh
{
public:
    /// @brief 构造函数
    /// @param[in] counterPath 收集器路径
    explicit LPdh(IN const wstring& counterPath);

    /// @brief 析构函数
    ~LPdh();

    /// @brief 收集整形数据
    /// 该函数耗费指定的收集时间
    /// @param[in] time 收集时间, 单位毫秒
    /// @param[in] value 存储收集后的数据
    /// @return 成功返回true, 失败返回false
    bool CollectDataLong(IN unsigned long time, OUT long& value);

private:
    CPdh* m_pPdh; ///< PDH实现对象
};

#endif