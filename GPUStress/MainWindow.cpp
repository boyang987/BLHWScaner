
#include "MainWindow.h"

#include "ExeParam.h"


#define ZOOM_IN 1.005 // 放大系数
#define ZOOM_OUT 0.995 // 缩小系数

#define MAX_ITER 256 // 最大迭代次数

LGameWindow::LGameWindow()
{
    


}

LGameWindow::~LGameWindow()
{
    if (m_image.PData != NULL)
    {
        delete[] m_image.PData;
        m_image.PData = 0;
    }

}

void LGameWindow::InitGame()
{
    int width = this->GetClientWidth();
    int height = this->GetClientHeight();

    // 初始化背景缓冲区
    HWND hWnd = this->GetWndHandle();
    m_backDC.Init(hWnd, width, height);
    
    // 设置最大迭代数
    m_paramFloat.MaxIter = MAX_ITER;
    m_paramDouble.MaxIter = MAX_ITER;

    // 申请图像需要的内存
    m_image.Width = width;
    m_image.Height = height;
    m_image.PData = new unsigned int[m_image.Width * m_image.Height];

    // 初始化距离为0.999
    m_dis = 0.999;
    // 初始化为缩小系数
    m_scale = ZOOM_OUT;

    CenterPoint cPoint;
    cPoint.X = -1.71070602293477591616;
    cPoint.Y  = -0.00045779170422538751;
    m_cPointList.push_back(cPoint);

    cPoint.X = -0.84001874276769517014;
    cPoint.Y = 0.22430391745363548184;
    m_cPointList.push_back(cPoint);

    cPoint.X = -1.54111536041941143971;
    cPoint.Y = 0.00000000000000126555;
    m_cPointList.push_back(cPoint);

    cPoint.X = -0.56220389591658670447;
    cPoint.Y = 0.64281771463971582037;
    m_cPointList.push_back(cPoint);

    m_cPointIndex = 0;

    GetDefaultAccelerator(m_defaultAcc);
}

void LGameWindow::RunGame()
{
    static unsigned int g_changeCount = 0;

    // 取出中心点位置
    CenterPoint cPoint = m_cPointList[m_cPointIndex];

    // 曼德勃罗特计算空间
    m_paramFloat.RealMin = float(cPoint.X - m_dis);
    m_paramFloat.RealMax = float(cPoint.X + m_dis);
    m_paramFloat.ImgMin = float(cPoint.Y - m_dis);
    m_paramFloat.ImgMax = float(cPoint.Y + m_dis);

    m_paramDouble.RealMin = cPoint.X - m_dis;
    m_paramDouble.RealMax = cPoint.X + m_dis;
    m_paramDouble.ImgMin = cPoint.Y - m_dis;
    m_paramDouble.ImgMax = cPoint.Y + m_dis;

    if (m_defaultAcc.SupportDouble)
    {
        AccGenerateMandelbrot(m_paramDouble, m_image);
        // 如果距离值达到最小, 则缩放系数改为放大
        if (m_dis <= 0.000000000001)
        {
            m_scale = ZOOM_IN;
            g_changeCount++;
        }
    }
    else
    {
        AccGenerateMandelbrot(m_paramFloat, m_image);
        // 如果距离值达到最小, 则缩放系数改为放大
        if (m_dis <= 0.00001)
        {
            m_scale = ZOOM_IN;
            g_changeCount++;
        }
    }
    
    
    // 如果距离值达到最大, 则缩放系数改为缩小
    if (m_dis >= 1.0)
    {
        m_scale = ZOOM_OUT;
        g_changeCount++;
    }

    m_dis = m_dis * m_scale;

    // 切换到别的中心点
    if (g_changeCount == 2)
    {
        g_changeCount = 0;

        m_cPointIndex++;
        if (m_cPointIndex == m_cPointList.size())
            m_cPointIndex = 0;
    }
}

void LGameWindow::PaintGame()
{
    m_backDC.Clear(255, 255, 255);

    HDC hBackDC = m_backDC.GetBackDC();

    int width = this->GetClientWidth();
    int height = this->GetClientHeight();

    SetStretchBltMode(hBackDC, COLORONCOLOR);
    BITMAPINFO  bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;
    bmi.bmiHeader.biSizeImage = 0;
    StretchDIBits(hBackDC, 0, 0, width, height, 0, 0, width, height, m_image.PData, &bmi, DIB_RGB_COLORS, SRCCOPY);

    // 绘制文本
    char strText[256] = {0};
    
    SetTextColor(hBackDC, RGB(255, 0, 0));
    SetBkMode(hBackDC, TRANSPARENT); // 设置文本背景透明

    int length = sprintf_s(strText, 256, "FPS: %u", m_fps);
    TextOutA(hBackDC, 10, 10, strText, length);

    length = sprintf_s(strText, 256, "Time: %.1lfM", m_timeM);
    TextOutA(hBackDC, 10, 30, strText, length);

    length = sprintf_s(strText, 256, "IsEmulated: %s", m_defaultAcc.IsEmulated ? "True" : "False");
    TextOutA(hBackDC, 10, 50, strText, length);

    length = sprintf_s(strText, 256, "SupportDouble: %s", m_defaultAcc.SupportDouble ? "True" : "False");
    TextOutA(hBackDC, 10, 70, strText, length);

    length = sprintf_s(strText, 256, "Device: %ws", m_defaultAcc.DeviceDesc.c_str());
    TextOutA(hBackDC, 10, 90, strText, length);

    SetBkMode(hBackDC, OPAQUE); // 设置文本背景不透明

    m_backDC.CopyToFrontDC();

}

void LGameWindow::Exe(IN unsigned int runTime)
{
	this->InitGame();

	bool bDone = false;
	
    m_timeM = 0.0;
    m_fps = 0;
    m_frameCount = 0;
    
    LTimer timer;

    MSG msg;
	while(!bDone)
	{
        if (m_frameCount == 0)
        {
            timer.Start();
        }
        m_frameCount++;

		while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
		{
			if( msg.message == WM_QUIT ) 
			{
				bDone = true;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}


        this->RunGame();

        // 刷新UI
        InvalidateRect(this->GetWndHandle(), NULL, TRUE);
        UpdateWindow(this->GetWndHandle());

        timer.End();
        if (timer.Time() >= 1000.0)
        {
            m_fps = (unsigned int)(m_frameCount * 1000 / timer.Time());
            m_frameCount = 0;

            // 累计耗时
            m_timeM += timer.Time() / 60000.0;
            if (m_timeM >= runTime)
                bDone = true;
        }

        
	}
}



LRESULT LGameWindow::MessageProc(IN UINT message, IN WPARAM wParam, IN LPARAM lParam)
{
	switch (message)
	{	
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	case WM_SIZE:
		{
			// 窗口大小改变重新初始化后备缓冲区
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			this->SetSize(width, height);
			HWND hWnd = this->GetWndHandle();
			m_backDC.Init(hWnd, width, height);
		}
		break;
	case WM_PAINT:
		{
			this->PaintGame();
		}
		break;
	default:
		break;
	}

	return DefWindowProc(GetWndHandle(), message, wParam, lParam);
}

int LMain()
{
    
    string cmdLine;
    LApParam::GetCmdLine(cmdLine);
    if (cmdLine.empty())
    {
        // 命令行为空时, 如果参数文件不存在则创建参数文件
        if (!IsParamFileExist())
            GenerateDefaultParamFile();
    }
    else if (cmdLine.compare("GETDEV") == 0)
    {
        // 命令行为GETDEV时, 只生成参数文件
        // 然后直接退出
        GenerateDefaultParamFile();
        return 0;
    }

    // 设置加速器
    ExeParam exeParam;
    GetExeParam(exeParam);
    if (!exeParam.DevicePath.empty())
    {
        SetDefaultAccelerator(exeParam.DevicePath);
    }
    

	LGameWindow mainWnd;
    mainWnd.SetSize(500, 500);
    mainWnd.SetSizingBorder(false);
    mainWnd.SetShowSysMenu(true);
    mainWnd.SetShowMinimizeBox(false);
    mainWnd.SetShowMaximizeBox(false);
    mainWnd.SetShowCaption(true);
    mainWnd.SetTitle("GPU Stress");

	mainWnd.Show();
	mainWnd.Exe(exeParam.RunTime);

	return 0;
}