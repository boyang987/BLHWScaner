
#ifndef _LWINDOWGDI_H_
#define _LWINDOWGDI_H_

#include <string>
using std::string;

#include <Windows.h>

//#include "LMacroDefine.h"

/// @brief 应用程序参数类
///
/// 
class LApParam
{
public:
	/// @brief 获取应用程序实例句柄
	/// @return 应用程序实例句柄
	static HINSTANCE GetInstance()
	{
		return m_hInstance;
	}

	/// @brief 获取应用程序命令行参数
	/// @param[in] cmdLine 命令行参数
	static void GetCmdLine(OUT string& cmdLine)
	{
		cmdLine.clear();
		cmdLine.append(m_cmdLine);
	}

	/// @brief 获取应用程序显示参数
	/// @return 应用程序显示参数
	static int GetCmdShow()
	{
		return m_cmdShow;
	}

private:
	static HINSTANCE m_hInstance;  ///< 应用程序实例句柄
	static string m_cmdLine;  ///< 命令行参数
	static int m_cmdShow;  ///< 显示参数

	friend int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, 
		IN LPSTR lpCmdLine, IN int nShowCmd );
};

/// @brief 自定义窗口基类
/// 
/// 
class LWindow
{
public:
	LWindow();
	virtual ~LWindow();
   
	/// @brief 设置窗口是否可以改变大小
	/// @param[in] bRet true(可以改变大小) false(不可以改变大小)
	void SetSizingBorder(IN bool bRet);

    /// @brief 设置窗口是否显示标题
    /// @param[in] bRet true(显示标题) false(不显示标题)
    void SetShowCaption(IN bool bRet);

    /// @brief 设置窗口是否显示系统菜单
    ///
    /// 设置显示系统菜单必需同时设置显示标题
    /// @param[in] bRet true(显示系统菜单) false(不显示系统菜单)
    void SetShowSysMenu(IN bool bRet);

	/// @brief 设置窗口显示最小化按钮
    ///
    /// 设置显示最小化按钮必需同时设置显示系统菜单
	/// @param[in] bRet true(显示最小化按钮) false(不显示最小化按钮)
	void SetShowMinimizeBox(IN bool bRet);

	/// @brief 设置窗口显示最大化按钮
    ///
    /// 设置显示最大化按钮必需同时设置显示系统菜单
	/// @param[in] bRet true(显示最大化按钮) false(不显示最大化按钮)
	void SetShowMaximizeBox(IN bool bRet);

    /// @brief 设置窗口位置(默认居中显示)
    /// @param[in] x X轴
    /// @param[in] y Y轴
    void SetPosition(IN int x, IN int y);

	/// @brief 设置窗口大小
	/// @param[in] width 宽度
	/// @param[in] height 高度
	void SetSize(IN int width, IN int height);

	/// @brief 设置窗口标题
	/// @param[in] title 标题
	void SetTitle(IN const string& title);

	/// @brief 获取窗口宽度
	/// @return 窗口宽度
	int GetWidth();

	/// @brief 获取窗口高度
	/// @return 窗口高度
	int GetHeight();

    /// @brief 获取客户区窗口宽度
    /// @return 客户区宽度
    int GetClientWidth();

    /// @brief 获取客户区窗口高度
    /// @return 客户区高度
    int GetClientHeight();

	/// @brief 获取窗口句柄
	/// @return 窗口句柄
	HWND GetWndHandle();

	/// @brief 显示窗口
	void Show();

	/// @brief 进入消息循环
	///
	/// 只有主窗口在LMain函数中才可调用该函数
	virtual void Exe();

private:
	/// @brief 创建窗口
	///
	/// 该方法会被Show()在第一次被调用时调用
	void Create();

	/// @brief 窗口自己的消息处理方法
	///
	/// 该方法接受不到WM_CREATE消息
	/// @param[in] message
	/// @param[in] wParam
	/// @param[in] lParam
	/// @return 
	virtual LRESULT MessageProc(IN UINT message, IN WPARAM wParam, IN LPARAM lParam);

	/// @brief 窗口过程回调函数
	/// @param[in] hwnd 窗口句柄
	/// @param[in] message
	/// @param[in] wParam
	/// @param[in] lParam
	/// @return 
	static LRESULT CALLBACK WindowProc (IN HWND hwnd, IN UINT message, IN WPARAM wParam, IN LPARAM lParam);

	LWindow(const LWindow&);
	void operator = (const LWindow&);

private:
	static int m_wndCount; ///< 窗口计数

private:
	WNDCLASSEX m_wndClassEx; ///< 窗口类结构
	HWND m_hWnd;  ///< 窗口句柄

	bool m_bWndHadCreated; ///< 窗口是否被创建
	string m_wndClassName; ///< 窗口类名称
	string m_wndTitle; ///< 窗口标题
	int m_wndHeight; ///< 窗口高度
	int m_wndWidth; ///< 窗口宽度

    bool m_bUseDefaultPos; ///< 窗口是否使用默认位置(居中)
    int m_wndPosX; ///< 窗口位置X坐标
    int m_wndPosY; ///< 窗口位置Y坐标

	DWORD m_windowStyle; ///< 窗口风格
};

/// @后备缓冲区DC类
class LBackBufferDC
{
public:
	LBackBufferDC();
	~LBackBufferDC();

	/// @brief 初始化后备缓冲区
	/// @param[in] hWnd 窗口句柄
	/// @param[in] width 后备缓冲区宽度
	/// @param[in] height 后备缓冲区高度
	void Init(IN HWND hWnd, IN int width, IN int height);

	/// @brief 以指定背景颜色来清除后备缓冲区
	/// @param[in] red
	/// @param[in] green
	/// @param[in] blue
	/// @return 错误返回false
	bool Clear(BYTE red, BYTE green, BYTE blue);

	/// @brief 复制后备缓冲区中的内容到前台缓冲区
	void CopyToFrontDC();

	/// @brief 获取后备缓冲区设备描述表句柄
	/// @return 后备缓冲区设备描述表句柄
	HDC GetBackDC();

private:
	HWND m_hWnd; ///< 窗口句柄
	int m_width; ///< 后备缓冲区宽度
	int m_height; ///< 后备缓冲区高度
	HDC m_hBackDC; ///< 后备缓冲区设备描述表句柄
	HBITMAP m_hBitMap; ///< 后备缓冲区使用的位图
	HBITMAP m_hOldBitMap; ///< 后备缓冲区原始位图
};

/// @高精度计时器类
class LTimer
{
public:
    LTimer()
    {
        // 获取CPU的时钟频率(即每秒的滴答数)
        QueryPerformanceFrequency((LARGE_INTEGER*)&m_performanceFreq);
    }

    /// @brief 开始计时
    void Start()
    {
        m_startTime = 0;
        m_time = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
    }

    /// @brief 结束计时
    void End()
    {
        LONGLONG currentTime = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
        m_time = currentTime - m_startTime;
    }

    /// @brief 获取计时时间
    /// @return 计时时间(单位毫秒)
    double Time()
    {
        double time = 0.0f;
        time = (double)(m_time * 1000) / (double)m_performanceFreq;
        return time;
    }

private:
    LONGLONG m_time; ///< 计时时间 
    LONGLONG m_startTime; ///< 计时器开始时间 
    LONGLONG m_performanceFreq; ///< CPU时钟频率(即每秒钟的滴答数)
};
#endif