
#include "LWindowGDI.h"

#include <string>
using std::string;
#include <sstream>
using std::stringstream;


#ifndef SAFE_DELETE_GDI
#define SAFE_DELETE_GDI(p) \
do\
{\
    if ((p) != NULL)\
    { \
        DeleteObject(p);\
        p = NULL; \
    }\
}while(0)
#endif

template<typename T>
string ConvertToString(IN T value)
{
    stringstream strStream;
    strStream << value;
    return strStream.str();
}

HINSTANCE LApParam::m_hInstance = NULL;
string LApParam::m_cmdLine;
int LApParam::m_cmdShow = 0;

int LMain();
int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
	LApParam::m_hInstance = hInstance;
	LApParam::m_cmdLine.append(lpCmdLine);
	LApParam::m_cmdShow = nShowCmd;

	return LMain();
}

int LWindow::m_wndCount = 0;

LWindow::LWindow()
{
	m_hWnd = NULL;

	ZeroMemory(&m_wndClassEx, sizeof(m_wndClassEx));
	m_wndClassEx.cbSize = sizeof(WNDCLASSEX);
	m_wndClassEx.style = CS_HREDRAW | CS_VREDRAW;  // 窗口样式
	m_wndClassEx.lpfnWndProc = WindowProc;
	m_wndClassEx.cbClsExtra = 0;
	m_wndClassEx.cbWndExtra = 0;
	m_wndClassEx.hInstance = NULL;  // 应用程序实例句柄
	m_wndClassEx.hIcon = LoadIcon (NULL, IDI_APPLICATION);  // 应用程序图标
	m_wndClassEx.hCursor = LoadCursor (NULL, IDC_ARROW);  // 鼠标光标
	m_wndClassEx.hbrBackground = NULL;  // 客户区背景色 使用双缓冲解决屏幕闪烁问题,需要将背景设为NULL
	m_wndClassEx.lpszMenuName  = NULL;  // 菜单的名字
	m_wndClassEx.lpszClassName = NULL;  // 窗口类名称
	m_wndClassEx.hIconSm = m_wndClassEx.hIcon;

	/*
	使用窗口计数, 自动创建窗口类名称
	*/
	m_wndClassName.append("LWindow");
	m_wndClassName.append(ConvertToString(m_wndCount));
	m_wndCount++;

	m_wndHeight = 600;
	m_wndWidth = 800;

    m_bUseDefaultPos = true;
    m_wndPosX = 0;
    m_wndPosY = 0;

	m_bWndHadCreated = false;
    m_windowStyle = WS_POPUP;
}

LWindow::~LWindow()
{
	UnregisterClassA(m_wndClassName.c_str(), LApParam::GetInstance());
}

void LWindow::SetSizingBorder(IN bool bRet)
{
	if (bRet)
		m_windowStyle = m_windowStyle | WS_THICKFRAME;
	else
		m_windowStyle = m_windowStyle & (~WS_THICKFRAME);
}

void LWindow::SetShowCaption(IN bool bRet)
{
    if (bRet)
        m_windowStyle = m_windowStyle | WS_CAPTION;
    else
        m_windowStyle = m_windowStyle & (~WS_CAPTION);
}

void LWindow::SetShowSysMenu(IN bool bRet)
{
    if (bRet)
        m_windowStyle = m_windowStyle | WS_SYSMENU;
    else
        m_windowStyle = m_windowStyle & (~WS_SYSMENU);
}

void LWindow::SetShowMinimizeBox(IN bool bRet)
{
	if (bRet)
		m_windowStyle = m_windowStyle | WS_MINIMIZEBOX;
	else
		m_windowStyle = m_windowStyle & (~WS_MINIMIZEBOX);
}

void LWindow::SetShowMaximizeBox(IN bool bRet)
{
	if (bRet)
		m_windowStyle = m_windowStyle | WS_MAXIMIZEBOX;
	else
		m_windowStyle = m_windowStyle & (~WS_MAXIMIZEBOX);
}

void LWindow::SetPosition(IN int x, IN int y)
{
    m_bUseDefaultPos = false;
    m_wndPosX = x;
    m_wndPosY = y;
}

void LWindow::SetSize(IN int width, IN int height)
{
	m_wndWidth = width;
	m_wndHeight = height;
}

void LWindow::SetTitle(IN const string& title)
{
	m_wndTitle.clear();
	m_wndTitle = title;
}

int LWindow::GetWidth()
{
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    return (int)(rect.right-rect.left);
}

int LWindow::GetHeight()
{
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    return (int)(rect.bottom-rect.top);
}

int LWindow::GetClientWidth()
{
    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);
    return (int)(clientRect.right-clientRect.left);
}

int LWindow::GetClientHeight()
{
    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);
    return (int)(clientRect.bottom-clientRect.top);
}

HWND LWindow::GetWndHandle()
{
	return m_hWnd;
}

void LWindow::Show()
{
	if (!m_bWndHadCreated)
	{
		this->Create();
		m_bWndHadCreated = true;
	}

	int cmdShow = LApParam::GetCmdShow();
	ShowWindow(m_hWnd, cmdShow);
	UpdateWindow (m_hWnd);

}

void LWindow::Exe()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void LWindow::Create()
{
	m_wndClassEx.hInstance = LApParam::GetInstance();
	m_wndClassEx.lpszClassName = m_wndClassName.c_str();

	DWORD dwRet = RegisterClassEx(&m_wndClassEx);
	//LASSERT(dwRet != 0);

    // 窗口居中显示
    if (m_bUseDefaultPos)
    {
        m_wndPosX = GetSystemMetrics(SM_CXFULLSCREEN)/2-m_wndWidth/2;
        m_wndPosY =GetSystemMetrics(SM_CYFULLSCREEN)/2-m_wndHeight/2;
    }

	m_hWnd = CreateWindowEx(
        WS_EX_APPWINDOW,
		m_wndClassName.c_str(), 
		m_wndTitle.c_str(),
		m_windowStyle,  // 窗口风格
		m_wndPosX,  // 窗口位置X
		m_wndPosY,  // 窗口位置Y 
		m_wndWidth,  // 窗口宽
		m_wndHeight,  // 窗口宽
		NULL,  // 父窗口句柄
		NULL,  // 菜单句柄
		m_wndClassEx.hInstance,
		NULL); 

	//LASSERT(m_hWnd != NULL);

	// 记录指针, 该运行在CreateWindow方法之后, 所以类自己的消息处理函数接受不到WM_CREATE消息
	SetWindowLongW(m_hWnd, GWL_USERDATA, (LONG)this);

}

LRESULT LWindow::MessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}

	return DefWindowProc(m_hWnd, message, wParam, lParam);
}

LRESULT CALLBACK LWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LWindow* pWindow = NULL;
	pWindow = (LWindow*)GetWindowLongW(hwnd, GWL_USERDATA);

	if (pWindow == NULL)
		return DefWindowProc(hwnd, message, wParam, lParam);
	else
		return pWindow->MessageProc(message, wParam, lParam);
}

LBackBufferDC::LBackBufferDC()
{
	m_hWnd = NULL;
	m_width = NULL;
	m_height = 0;
	m_hBitMap = NULL;
	m_hOldBitMap = NULL;
	// 创建兼容的DC
	m_hBackDC = CreateCompatibleDC(NULL);
}

LBackBufferDC::~LBackBufferDC()
{
	if (m_hOldBitMap != NULL)
	{
		SelectObject(m_hBackDC, m_hOldBitMap);
	}
	SAFE_DELETE_GDI(m_hBitMap);

	DeleteDC(m_hBackDC);
}

bool LBackBufferDC::Clear(BYTE red, BYTE green, BYTE blue)
{
	BOOL bRet = BitBlt(m_hBackDC, 0, 0, m_width, m_height, NULL, NULL, NULL, BLACKNESS);
    if (0 == bRet)
        return false;

    // 以指定颜色来填充背景
    HBRUSH hbkBrush = CreateSolidBrush(RGB(red, green, blue));
    RECT rect;
    rect.top = 0;
    rect.left = 0;
    rect.right = m_width;
    rect.bottom = m_height;
 
    FillRect(m_hBackDC, &rect, hbkBrush);
    DeleteObject(hbkBrush);

    return true;
}

HDC LBackBufferDC::GetBackDC()
{
	return m_hBackDC;
}

void LBackBufferDC::CopyToFrontDC()
{
	HDC hdc = GetDC(m_hWnd);
	BitBlt(hdc, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);
}

void LBackBufferDC::Init(HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_width = width;
	m_height = height;

	if (m_hOldBitMap != NULL)
	{
		SelectObject(m_hBackDC, m_hOldBitMap);
	}

	SAFE_DELETE_GDI(m_hBitMap);

	HDC hdc = GetDC(hWnd);
	m_hBitMap = CreateCompatibleBitmap(hdc, width, height);
	m_hOldBitMap = (HBITMAP)SelectObject(m_hBackDC, m_hBitMap);
	ReleaseDC(hWnd, hdc);
}



