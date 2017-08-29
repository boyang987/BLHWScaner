

#ifndef _LWEBCAM_H_
#define _LWEBCAM_H_

#include <string>
using std::string;
using std::wstring;

#include <vector>
using std::vector;



#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


class CWebcam;


/// <SUMMARY>
/// 位图结构
/// </SUMMARY>
class CDBIImage
{
	friend class CWebcam;

public:
	CDBIImage();
	~CDBIImage();

	/// <SUMMARY>
	/// 获取整张图的平均亮度值
	/// </SUMMARY>
	/// <PARAM name = "avgLuma" dir = "OUT">
	/// 存储亮度值
	/// </PARAM>
	/// <RETURNS>
	/// 成功返回true, 失败返回false
	/// </RETURNS>
	bool GetAvgLuma(OUT int& avgLuma);

	/// <SUMMARY>
	/// 保存为BMP文件
	/// </SUMMARY>
	/// <PARAM name = "fileName" dir = "IN">
	/// 文件名
	/// </PARAM>
	/// <RETURNS>
	/// 成功返回true, 失败返回false
	/// </RETURNS>
	bool SaveToBMP(IN const char* fileName);

private:
	long Size; // 位图大小
	unsigned char* Data; // 位图数据

private:
	CDBIImage(const CDBIImage&);
	CDBIImage& operator = (const CDBIImage&);
};

/// <SUMMARY>
/// 摄像头接口
/// </SUMMARY>
class LWebcam
{
public:
    /// <SUMMARY>
    /// 析构函数
    /// </SUMMARY>
    virtual ~LWebcam() = 0 {}

    /// <SUMMARY>
    /// 设置父窗口
    /// </SUMMARY>
    /// <PARAM name = "hwnd" dir = "IN">
    /// 父窗口句柄
    /// </PARAM>
    /// <PARAM name = "left, top, width, height" dir = "IN">
    /// 摄像头窗口在父窗口中的位置和大小, 摄像头窗口左起点, 摄像头窗口上起点, 摄像头窗口宽度, 摄像头窗口高度
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool SetParentWindow(IN void* hwnd, IN long left, IN long top, IN long width, IN long height) = 0;

    /// <SUMMARY>
    /// 获取摄像头友好名称
    /// </SUMMARY>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储摄像头友好名称
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool GetFriendlyName(OUT wstring& name) = 0;

    /// <SUMMARY>
    /// 获取摄像头显示名称
    /// </SUMMARY>
    /// <PARAM name = "name" dir = "OUT">
    /// 存储摄像头显示名称
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool GetDisplayName(OUT wstring& name) = 0;

    /// <SUMMARY>
    /// 获取当前图像
    /// </SUMMARY>
    /// <PARAM name = "image" dir = "OUT">
    /// 存储图像信息
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool GetCurrentImage(OUT CDBIImage& image) = 0;

    /// <SUMMARY>
    /// 开启摄像头
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool Run() = 0;

    /// <SUMMARY>
    /// 暂停摄像头
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool Pause() = 0;

    /// <SUMMARY>
    /// 停止摄像头
    /// </SUMMARY>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool Stop() = 0;

    /// <SUMMARY>
    /// 获取平均帧速度(每秒)
    /// </SUMMARY>
    /// <PARAM name = "rate" dir = "IN">
    /// 存储帧速度
    /// </PARAM>
    /// <RETURNS>
    /// 成功返回true, 失败返回false, 使用GetErrorMessage()方法获取错误信息
    /// </RETURNS>
    virtual bool GetAvgFrameRate(int& rate) = 0;

    /// <SUMMARY>
    /// 获取错误信息
    /// </SUMMARY>
    /// <RETURNS>
    /// 错误信息
    /// </RETURNS>
    virtual  string GetErrorMessage() = 0;

    /// <SUMMARY>
    /// 获取错误信息
    /// </SUMMARY>
    virtual void GetErrorMessage(string& error) = 0;


    /// <SUMMARY>
    /// 释放资源
    /// </SUMMARY>
    virtual void Release() = 0;
};

/// <SUMMARY>
/// 枚举摄像头
/// </SUMMARY>
/// <PARAM name = "webcamList" dir = "OUT">
/// 存储摄像头对象
/// </PARAM>
/// <PARAM name = "errorMessage" dir = "OUT">
/// 存储错误信息
/// </PARAM>
/// <RETURNS>
/// 成功返回true, 失败返回false
/// </RETURNS>
bool LEnumWebcam(OUT vector<LWebcam*>& webcamList, string& errorMessage);


#endif