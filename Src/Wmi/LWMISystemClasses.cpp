#include "LWMISystemClasses.h"

#include "LWMICoreManager.h"

namespace LWMI
{
    LComputerSystemManager::LComputerSystemManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_ComputerSystem");
    }
    LComputerSystemManager::~LComputerSystemManager()
    {
        delete m_pWMICoreManager;
    }

    int LComputerSystemManager::GetComputerSystemCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LComputerSystemManager::GetComputerSystemModel(IN int index, OUT wstring& model)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Model", model);
    }

    bool LComputerSystemManager::GetComputerSystemManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LComputerSystemManager::GetComputerSystemPCType(IN int index, OUT LCOMPUTER_SYSTEM_TYPE& type)
    {
        type = COMPUTER_SYSTEM_UNKNOWN;

        LUINT16 u16Type;
        bool bRet = m_pWMICoreManager->GetUINT16Property(index, L"PCSystemType", u16Type);
        LUINT16 u16TypeEx;
        bool bRetEx = m_pWMICoreManager->GetUINT16Property(index, L"PCSystemTypeEx", u16TypeEx);

        if (bRet)
        {
            if (1 == u16Type)
                type = COMPUTER_SYSTEM_DESKTOP;

            if (2 == u16Type)
                type = COMPUTER_SYSTEM_NOTE_BOOK;
        }

        if (bRetEx)
        {
            if (8 == u16TypeEx)
                type = COMPUTER_SYSTEM_TABLET;
        }

        return bRet;
    }

    bool LComputerSystemManager::GetComputerSystemType(IN int index, OUT wstring& type)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemType", type);
    }

    LOperatingSystemManager::LOperatingSystemManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_OperatingSystem");
    }

    LOperatingSystemManager::~LOperatingSystemManager()
    {
        delete m_pWMICoreManager;
    }

    int LOperatingSystemManager::GetOSCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LOperatingSystemManager::GetOSCaption(IN int index, OUT wstring& caption)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Caption", caption);
    }

    bool LOperatingSystemManager::GetOSArchitecture(IN int index, OUT wstring& architecture)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"OSArchitecture", architecture);
    }

    bool LOperatingSystemManager::GetOSVersion(IN int index, OUT wstring& version)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Version", version);
    }

    bool LOperatingSystemManager::GetOSSystemDrive(IN int index, OUT wstring& drive)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemDrive", drive);
    }

    LMS_SystemInformationManager::LMS_SystemInformationManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MS_SystemInformation");
    }

    LMS_SystemInformationManager::~LMS_SystemInformationManager()
    {
        delete m_pWMICoreManager;
    }

    int LMS_SystemInformationManager::GetSystemInforCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMS_SystemInformationManager::GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BaseBoardManufacturer", manufacturer);
    }

    bool LMS_SystemInformationManager::GetBaseBoardProductName(IN int index, OUT wstring& product)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BaseBoardProduct", product);
    }

    bool LMS_SystemInformationManager::GetBIOSReleaseDate(IN int index, OUT wstring& releaseDate)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSReleaseDate", releaseDate);
    }

    bool LMS_SystemInformationManager::GetBIOSVendor(IN int index, OUT wstring& vendor)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSVendor", vendor);
    }

    bool LMS_SystemInformationManager::GetBIOSVersion(IN int index, OUT wstring& version)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSVersion", version);
    }

    bool LMS_SystemInformationManager::GetSystemFamily(IN int index, OUT wstring& family)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemFamily", family);
    }

    bool LMS_SystemInformationManager::GetSystemManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemManufacturer", manufacturer);
    }

    bool LMS_SystemInformationManager::GetSystemProductName(IN int index, OUT wstring& productName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemProductName", productName);
    }

    bool LMS_SystemInformationManager::GetSystemSKU(IN int index, OUT wstring& sku)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemSKU", sku);
    }

    LMSSmBios_RawSMBiosTablesManager::LMSSmBios_RawSMBiosTablesManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MSSmBios_RawSMBiosTables");
    }

    LMSSmBios_RawSMBiosTablesManager::~LMSSmBios_RawSMBiosTablesManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LMSSmBios_RawSMBiosTablesManager::GetRawSMBiosTablesCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosMajorVersion(IN int index, OUT unsigned char& version)
    {
        return m_pWMICoreManager->GetUINT8Property(index, L"SmbiosMajorVersion", version);
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosMinorVersion(IN int index, OUT unsigned char& version)
    {
        return m_pWMICoreManager->GetUINT8Property(index, L"SmbiosMinorVersion", version);
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosData(IN int index, OUT vector<unsigned char>& data)
    {
        return m_pWMICoreManager->GetUINT8ArrayProperty(index, L"SMBiosData", data);
    }

    LPerfRawData_PerfOS_MemoryManager::LPerfRawData_PerfOS_MemoryManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_PerfRawData_PerfOS_Memory");
    }

    LPerfRawData_PerfOS_MemoryManager::~LPerfRawData_PerfOS_MemoryManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LPerfRawData_PerfOS_MemoryManager::GetMemoryPerfDataCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPerfRawData_PerfOS_MemoryManager::GetMemoryAvailableMBytes(IN int index, OUT unsigned long& availableBytes)
    {
        LUINT64 temp;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"AvailableMBytes", temp);
        if (bRet)
            availableBytes = (unsigned long)temp;

        return bRet;
    }

    bool LPerfRawData_PerfOS_MemoryManager::GetMemoryUnusedMBytes(IN int index, OUT unsigned long& unusedBytes)
    {
        LUINT64 temp;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"FreeAndZeroPageListBytes", temp);
        if (bRet)
        {
            unusedBytes = (unsigned long)(temp/1024/1024);
        }

        return bRet;
    }

}

