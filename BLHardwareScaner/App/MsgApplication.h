
#pragma once


#include <QtGui/QApplication>


/// @brief 自定义应用程序类
class MsgApplication:public QApplication  
{  
	Q_OBJECT  

public:  
	/// @brief 构造函数
	MsgApplication(int argc,char *argv[]);

	/// @brief 析构函数
	~MsgApplication();

protected:  
	/// @brief Windows事件过滤函数
	bool winEventFilter(MSG* pMSG, long* pResult);

signals:
	/// @brief 系统进入挂起状态前的信号
	void EnterSuspend();

	/// @brief 系统从挂起中恢复信号
	void ResumeSuspend();
	
};  