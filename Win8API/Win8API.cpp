
#define _EXPORTING_WIN8API
#include "Win8API.h"

#include <Windows.h>
#include <WinBase.h>
#include <WinNT.h>

int GetBIOSMode()
{
    FIRMWARE_TYPE firmWare;
    if (FALSE == GetFirmwareType(&firmWare))
        return BIOS_UNKNOWN;

    if (firmWare == FIRMWARE_TYPE::FirmwareTypeBios)
        return BIOS_LEGACY;

    if (firmWare == FIRMWARE_TYPE::FirmwareTypeUefi)
        return BIOS_UEFI;

    return BIOS_UNKNOWN;
}

