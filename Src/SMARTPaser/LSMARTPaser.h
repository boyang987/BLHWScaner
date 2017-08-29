

#ifndef _LSMARTPASER_H_
#define _LSMARTPASER_H_

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief SMART数据长度
#ifndef SMART_DATA_LENGTH
#define SMART_DATA_LENGTH 362
#endif



class CSMARTParser;

/// @brief SMART解析类
class LSMARTParser
{
public:
    /// @brief 构造函数
    /// @param[in] smartData[362] 要求输入362个字节的SMART数据
    LSMARTParser(IN unsigned char smartData[SMART_DATA_LENGTH]);
    ~LSMARTParser();

    /// @brief 获取温度值, 单位摄氏度
    /// @param[out] temp 存储温度值
    /// @return 成功返回true, 失败返回false
    bool GetTemperature(OUT unsigned int& temp);

    /// @brief 获取通电时间, 单位小时
    ///  
    /// @param[out] hours 存储通电时间, 单位小时(hours)
    /// @return 成功返回true, 失败返回false
    bool GetPowerOnHours(OUT unsigned long& hours);

private:
    CSMARTParser* m_pSMARTParser; ///< SMART解析实现对象
};

#endif