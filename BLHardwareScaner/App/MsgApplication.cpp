#include "MsgApplication.h"

#include <Windows.h>

#include "..\\Src\\Log\\LLog.h"

MsgApplication::MsgApplication(int argc,char *argv[])
	:QApplication(argc,argv)  
{

}

MsgApplication::~MsgApplication()
{

}

bool MsgApplication::winEventFilter(MSG* pMSG, long* pResult)
{
	switch (pMSG->message)
	{
	case WM_POWERBROADCAST:
		if (pMSG->wParam == PBT_APMRESUMESUSPEND)
		{
			PrintLogA("MsgApplication: Resume From S3 Or S4");
			emit ResumeSuspend();
			return true;
		}
		else if (pMSG->wParam == PBT_APMSUSPEND)
		{
			PrintLogA("MsgApplication: Enter S3 Or S4");
			emit EnterSuspend();
			return true;
		}

		break;
	default:
		break;
	}

	return false;
}