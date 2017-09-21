
#include "PowerMgmt.h"

#include <Windows.h>
#include <powrprof.h>

#pragma comment(lib, "powrprof.lib")

void PowerMgmt::EnterHibernate()
{
    SetSuspendState(TRUE, TRUE, FALSE);
}


void PowerMgmt::EnterSleep()
{
    SetSuspendState(FALSE, TRUE, FALSE);
}