
#include "LPdh.h"

#include <Pdh.h>


#pragma comment(lib, "Pdh.lib")


/// @brief PDH接口类
class CPdh
{
public:
    /// @brief 构造函数
    /// @param[in] counterPath 收集器路径
    explicit CPdh(IN const wstring& counterPath)
    {
        m_hQuery = NULL;
        m_hCounter = NULL;


        PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &m_hQuery);
        if (lStatus != ERROR_SUCCESS)
        {
            return;
        }

        PdhAddCounterW(m_hQuery, counterPath.c_str(), NULL, &m_hCounter);
    }

    /// @brief 析构函数
    ~CPdh()
    {
        if (NULL != m_hCounter)
        {
            PdhRemoveCounter(m_hCounter);
            m_hCounter = NULL;
        }

        if (NULL != m_hQuery)
        {
            PdhCloseQuery(m_hQuery);
            m_hQuery = NULL;
        }
       
        
    }

    /// @brief 收集整形数据
    /// 该函数耗费指定的收集时间
    /// @param[in] time 收集时间, 单位毫秒
    /// @param[in] value 存储收集后的数据
    /// @return 成功返回true, 失败返回false
    bool CollectDataLong(IN unsigned long time, OUT long& value)
    {
        if (NULL == m_hCounter)
            return false;

        PDH_STATUS lStatus = PdhCollectQueryData(m_hQuery);
        if (lStatus != ERROR_SUCCESS)
            return false;

        Sleep(time);

        lStatus = PdhCollectQueryData(m_hQuery);
        if (lStatus != ERROR_SUCCESS)
            return false;

        PDH_FMT_COUNTERVALUE counterValue;
        lStatus = PdhGetFormattedCounterValue(m_hCounter, PDH_FMT_LONG, NULL, &counterValue);
        if (lStatus != ERROR_SUCCESS)
        {
            return false;
        }

        value = counterValue.longValue;

        return true;
    }

private:
    HQUERY  m_hQuery; ///< 查询句柄
    HCOUNTER m_hCounter; ///< 收集器句柄
};

LPdh::LPdh(IN const wstring& counterPath)
{
    m_pPdh = new CPdh(counterPath);
}

LPdh::~LPdh()
{
    if (NULL != m_pPdh)
    {
        delete m_pPdh;
        m_pPdh = NULL;
    }
}

bool LPdh::CollectDataLong(IN unsigned long time, OUT long& value)
{
    return m_pPdh->CollectDataLong(time, value);
}