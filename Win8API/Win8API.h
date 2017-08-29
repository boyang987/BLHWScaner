
#ifndef _WIN8API_H_
#define _WIN8API_H_

#ifdef _EXPORTING_WIN8API
#define WIN8API_API_DECLSPEC __declspec(dllexport)
#else
#define WIN8API_API_DECLSPEC __declspec(dllimport)
#endif

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 未知模式
#ifndef BIOS_UNKNOWN
#define BIOS_UNKNOWN 0
#endif

/// @brief 传统模式
#ifndef BIOS_LEGACY
#define BIOS_LEGACY 1
#endif

/// @brief UEFI模式
#ifndef BIOS_UEFI
#define BIOS_UEFI 2
#endif


extern "C"
{
    /// @brief 获取BIOS模式
    /// @return 未知模式(BIOS_UNKNOWN 0), 传统模式(BIOS_LEGACY 1), UEFI模式(BIOS_UEFI 2)
    WIN8API_API_DECLSPEC int GetBIOSMode();
};

#endif