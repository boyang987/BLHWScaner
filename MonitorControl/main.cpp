
#include <string>
using std::string;

#include <Windows.h>


int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
    string cmd = lpCmdLine;
    if (cmd == "ON")
    {
        // 移动鼠标点亮屏幕
        mouse_event(MOUSEEVENTF_MOVE, 0, 1, 0, NULL);
        Sleep(50);
        mouse_event(MOUSEEVENTF_MOVE, 0, -1, 0, NULL);
    }
    else if (cmd == "OFF")
    {
        // 关闭屏幕
        SendMessageA(HWND_BROADCAST, WM_SYSCOMMAND,  SC_MONITORPOWER,  (LPARAM)2);  
    }
    
    Sleep(100);

    
    return 0;
}