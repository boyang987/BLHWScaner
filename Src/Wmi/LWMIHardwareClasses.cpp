
#include "LWMIHardwareClasses.h"

#include <Windows.h>

#include "LWMICoreManager.h"

namespace LWMI
{
    LBaseBoardManager::LBaseBoardManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_BaseBoard");
    }

    LBaseBoardManager::~LBaseBoardManager()
    {
        delete m_pWMICoreManager;
    }

    int LBaseBoardManager::GetBaseBoardCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LBaseBoardManager::GetBaseBoardCaption(IN int index, OUT wstring& caption)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Caption", caption);
    }

    bool LBaseBoardManager::GetBaseBoardDescription(IN int index, OUT wstring& description)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Description", description);
    }

    bool LBaseBoardManager::GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LBaseBoardManager::GetBaseBoardSerialNumber(IN int index, OUT wstring& serialNumber)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"SerialNumber", serialNumber);
    }

    LBIOSManager::LBIOSManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_BIOS");
    }

    LBIOSManager::~LBIOSManager()
    {
        delete m_pWMICoreManager;
    }

    int LBIOSManager::GetBIOSCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LBIOSManager::GetBIOSDescription(IN int index, OUT wstring& description)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Description", description);
    }

    bool LBIOSManager::GetBIOSManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LBIOSManager::GetBIOSSerialNumber(IN int index, OUT wstring& serialNumber)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"SerialNumber", serialNumber);
    }

    bool LBIOSManager::GetBIOSVersion(IN int index, OUT wstring& version)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"Version", version);
    }

    bool LBIOSManager::GetSMBIOSBIOSVersion(IN int index, OUT wstring& version)
    {
        return this->m_pWMICoreManager->GetStringProperty(index, L"SMBIOSBIOSVersion", version);
    }

    LProcessorManager::LProcessorManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_Processor");
    }

    LProcessorManager::~LProcessorManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LProcessorManager::GetProcessorCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LProcessorManager::GetProcessorName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LProcessorManager::GetProcessorDesription(IN int index, OUT wstring& description)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Description", description);
    }

    bool LProcessorManager::GetProcessorArchitecture(IN int index, OUT LPROCESSOR_ARCHITECTURE& architecture)
    {
        LUINT16 temp;
        bool bRet = m_pWMICoreManager->GetUINT16Property(index, L"Architecture", temp);
        if (bRet)
            architecture = (LPROCESSOR_ARCHITECTURE)temp;
        return bRet;
    }

    bool LProcessorManager::GetProcessorManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LProcessorManager::GetProcessorMaxClockSpeed(IN int index, OUT unsigned long& speed)
    {
        LUINT temp;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"MaxClockSpeed", temp);
        if (bRet)
            speed = temp;
        return bRet;
    }

    bool LProcessorManager::GetProcessorCoresNumber(IN int index, OUT unsigned long& coresNumber)
    {
        LUINT temp;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"NumberOfCores", temp);
        if (bRet)
            coresNumber = temp;
        return bRet;
    }

    bool LProcessorManager::GetProcessorLogicalProcessorNumber(IN int index, OUT unsigned long& logicalProcessorsNumber)
    {
        LUINT temp;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"NumberOfLogicalProcessors", temp);
        if (bRet)
            logicalProcessorsNumber = temp;
        return bRet;
    }

    bool LProcessorManager::GetProcessorLoadPercentage(IN int index, OUT unsigned long& percentage)
    {
        LUINT16 temp;
        bool bRet = m_pWMICoreManager->GetUINT16Property(index, L"LoadPercentage", temp);
        if (bRet)
            percentage = (unsigned long)temp;

        return bRet;
    }

    LPhysicalMemoryManager::LPhysicalMemoryManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_PhysicalMemory");
    }

    LPhysicalMemoryManager::~LPhysicalMemoryManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LPhysicalMemoryManager::GetPhysicalMemoryCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPhysicalMemoryManager::GetPhysicalMemoryDescription(IN int index, OUT wstring& description)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Description", description);
    }

    bool LPhysicalMemoryManager::GetPhysicalMemoryManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LPhysicalMemoryManager::GetPhysicalMemorySerialNumber(IN int index, OUT wstring& serialNumber)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SerialNumber", serialNumber);
    }

    bool LPhysicalMemoryManager::GetPhysicalMemoryCapacity(IN int index, OUT unsigned long& capacity)
    {
        LUINT64 temp;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"Capacity", temp);
        if (bRet)
        {
            temp = temp/1024; //K
            temp = temp/1024; // M
            capacity = (unsigned long)temp;
        }

        return bRet;
    }

    bool LPhysicalMemoryManager::GetPhysicalMemorySpeed(IN int index, OUT unsigned long& speed)
    {
        LUINT uSpeed;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"Speed", uSpeed);
        speed = uSpeed;

        return bRet;
    }

    bool LPhysicalMemoryManager::GetPhysicalMemoryPartNumber(IN int index, OUT wstring& partNumber)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"PartNumber", partNumber);
    }

    LVideoControllerManager::LVideoControllerManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_VideoController");
    }

    LVideoControllerManager::LVideoControllerManager(IN const wstring& pnpDeviceID)
    {
        // 查询语句中需要将"\\"字符替换为"\\\\"
        wstring pnpDeviceIDFormat;
        for (unsigned int i = 0; i < pnpDeviceID.length(); i++)
        {
            if (pnpDeviceID[i] == L'\\')
            {
                pnpDeviceIDFormat.push_back(L'\\');
                pnpDeviceIDFormat.push_back(L'\\');
            }
            else
            {
                pnpDeviceIDFormat.push_back(pnpDeviceID[i]);
            }
        }

        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);

        wstring queryStatement = L"SELECT * FROM Win32_VideoController ";
        queryStatement += L"WHERE PNPDeviceID = \"";
        queryStatement += pnpDeviceIDFormat;
        queryStatement += L"\"";

        bRet = m_pWMICoreManager->WQLQuery(queryStatement.c_str());
    }

    LVideoControllerManager::~LVideoControllerManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LVideoControllerManager::GetVideoControllerCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LVideoControllerManager::GetVideoControllerDescription(IN int index, OUT wstring& description)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Description", description);
    }

    bool LVideoControllerManager::GetVideoControllerAdapterRAMCapacity(IN int index, OUT unsigned long& ramCapacity)
    {
        LUINT temp;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"AdapterRAM", temp);
        if (bRet)
        {
            temp = temp/1024; // K
            temp = temp/1024; // M
            ramCapacity = temp;
        }

        return bRet;
    }

    LDiskDriveManager::LDiskDriveManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_DiskDrive");
    }

    LDiskDriveManager::~LDiskDriveManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LDiskDriveManager::GetDiskCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LDiskDriveManager::GetDiskModel(IN int index, OUT wstring& model)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Model", model);
    }

    bool LDiskDriveManager::GetDiskSerialNumber(IN int index, OUT wstring& serialNumber)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SerialNumber", serialNumber);
    }

    bool LDiskDriveManager::GetDiskSize(IN int index, OUT unsigned long& size)
    {
        LUINT64 size64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"Size", size64);

        size64 = size64/1024; // K
        size64 = size64/1024; // M
        size64 = size64/1024; // G

        size = (unsigned long)size64;

        return bRet;
    }

    bool LDiskDriveManager::GetDiskDeviceID(IN int index, OUT wstring& deviceID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"DeviceID", deviceID);
    }

    bool LDiskDriveManager::GetDiskPNPDeviceID(IN int index, OUT wstring& pnpDeviceID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"PNPDeviceID", pnpDeviceID);
    }

    bool LDiskDriveManager::GetDiskInterfaceType(IN int index, OUT wstring& type)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"InterfaceType", type);
    }

    bool LDiskDriveManager::GetDiskType(IN int index, OUT LDISK_TYPE& type)
    {
        bool bRet = false;
        HANDLE hDiskDrive = NULL;
        STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = NULL;

        wstring deviceId;
        wstring interfaceType;
        if (!this->GetDiskDeviceID(index, deviceId))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (!this->GetDiskInterfaceType(index, interfaceType))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 打开设备句柄
        hDiskDrive = CreateFileW(deviceId.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDiskDrive == INVALID_HANDLE_VALUE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        STORAGE_PROPERTY_QUERY propertyQuery;
        ZeroMemory(&propertyQuery, sizeof(propertyQuery));
        propertyQuery.PropertyId = StorageDeviceProperty;
        propertyQuery.QueryType = PropertyStandardQuery;

        ULONG nBytes;
        STORAGE_DEVICE_DESCRIPTOR deviceDescriptor;
        BOOL dwRet = DeviceIoControl(
            hDiskDrive, 
            IOCTL_STORAGE_QUERY_PROPERTY, 
            &propertyQuery, 
            sizeof(propertyQuery), 
            &deviceDescriptor, 
            sizeof(deviceDescriptor), 
            &nBytes, 
            NULL);

        if (dwRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        pDeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)malloc(nBytes);
        dwRet = DeviceIoControl(
            hDiskDrive, 
            IOCTL_STORAGE_QUERY_PROPERTY, 
            &propertyQuery, 
            sizeof(propertyQuery), 
            pDeviceDescriptor, 
            nBytes, 
            &nBytes, 
            NULL);

        if (dwRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (pDeviceDescriptor->RemovableMedia == TRUE)
        {
            type = REMOVABLE_DISK;
        }

        if (pDeviceDescriptor->RemovableMedia == FALSE)
        {
            switch (pDeviceDescriptor->BusType)
            {
            case BusTypeVirtual:
            case BusTypeFileBackedVirtual:
                type = VIRTUAL_DISK;
                break;
            default:
                if (interfaceType.find(L"USB") != wstring::npos)
                    type = EXTERNAL_USB_DISK;
                else if (interfaceType.find(L"IDE") != wstring::npos ||
                    interfaceType.find(L"SCSI") != wstring::npos)
                    type = FIXED_DISK;
                else
                    type = UNKNOWN_DISK;
                break;
            }
        }

        bRet = true;

SAFE_EXIT:
        if (hDiskDrive != NULL)
        {
            CloseHandle(hDiskDrive);
            hDiskDrive = NULL;
        }

        if (pDeviceDescriptor != NULL)
        {
            free(pDeviceDescriptor);
            pDeviceDescriptor = NULL;
        }

        return bRet;
    }

    bool LDiskDriveManager::GetDiskLogicalName(IN int index, OUT wstring& logicalName)
    {
        logicalName.clear();

        wstring diskDeviceID;
        bool bRet = this->GetDiskDeviceID(index, diskDeviceID);
        if (!bRet)
            return false;

        wstring partitionQuery;
        partitionQuery = L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='";
        partitionQuery += diskDeviceID;
        partitionQuery += L"'} WHERE AssocClass = Win32_DiskDriveToDiskPartition";

        LWMICoreManager diskPartitionManager;
        bRet = diskPartitionManager.BaseInit(NAMESPACE_ROOT_CIMV2);
        if (!bRet)
            return false;

        bRet = diskPartitionManager.WQLQuery(partitionQuery.c_str());
        if (!bRet)
            return false;

        for (int i = 0; i < diskPartitionManager.GetObjectsCount(); i++)
        {
            wstring partitionDeviceId;
            bRet = diskPartitionManager.GetStringProperty(i, L"DeviceID", partitionDeviceId);
            if (!bRet)
                continue;


            wstring logicalDiskQuery;
            logicalDiskQuery = L"ASSOCIATORS OF {Win32_DiskPartition.DeviceID='";
            logicalDiskQuery += partitionDeviceId;
            logicalDiskQuery += L"'} WHERE AssocClass = Win32_LogicalDiskToPartition";

            LWMICoreManager logicalDiskManager;
            bRet = logicalDiskManager.BaseInit(NAMESPACE_ROOT_CIMV2);
            if (!bRet)
                continue;

            bRet = logicalDiskManager.WQLQuery(logicalDiskQuery.c_str());
            if (!bRet)
                continue;
                

            for (int j = 0; j < logicalDiskManager.GetObjectsCount(); j++)
            {
                wstring name;
                bRet = logicalDiskManager.GetStringProperty(j, L"Name", name);
                if (!bRet)
                    continue;

                logicalName += name;
                logicalName += L";";
            }
        }

        // 删除最后一个分号
        if (!logicalName.empty())
        {
            logicalName.erase(logicalName.size()-1, 1);
        }

        return true;
    }


    LBatteryManager::LBatteryManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_Battery");
    }

    LBatteryManager::~LBatteryManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LBatteryManager::GetBatteryCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LBatteryManager::GetBatteryName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LBatteryManager::GetBatteryDeviceID(IN int index, OUT wstring& deviceID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"DeviceID", deviceID);
    }

    bool LBatteryManager::GetBatteryDesignVoltage(IN int index, OUT unsigned long& voltage)
    {
        LUINT64 uiVoltage;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"DesignVoltage", uiVoltage);
        if (bRet)
        {
            voltage = (unsigned long)uiVoltage;
        }

        return bRet;
    }

    LBatteryStaticDataManager::LBatteryStaticDataManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM BatteryStaticData");
    }

    LBatteryStaticDataManager::~LBatteryStaticDataManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LBatteryStaticDataManager::GetBatteryCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LBatteryStaticDataManager::GetBatteryManufacturerName(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"ManufactureName", manufacturer);
    }

    bool LBatteryStaticDataManager::GetBatteryUniqueID(IN int index, OUT wstring& uniqueID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"UniqueID", uniqueID);
    }

    bool LBatteryStaticDataManager::GetBatterySerialNumber(IN int index, OUT wstring& serialNumber)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SerialNumber", serialNumber);
    }

    bool LBatteryStaticDataManager::GetBatteryDesignedCapacity(IN int index, OUT unsigned long& capacity)
    {
        LUINT uiCapacity;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"DesignedCapacity", uiCapacity);
        if (bRet)
        {
            capacity = (unsigned long)uiCapacity;
        }

        return bRet;
    }

    LBatteryFullCapacityManager::LBatteryFullCapacityManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM BatteryFullChargedCapacity");
    }

    LBatteryFullCapacityManager::~LBatteryFullCapacityManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    bool LBatteryFullCapacityManager::GetBatteryFullChargedCapacity(IN int index, OUT unsigned long& capacity)
    {
        LUINT uiCapacity;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"FullChargedCapacity", uiCapacity);
        if (bRet)
        {
            capacity = (unsigned long)uiCapacity;
        }

        return bRet;
    }

    LNetworkAdapterManager::LNetworkAdapterManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_NetworkAdapter Where PhysicalAdapter = true");
    }

    LNetworkAdapterManager::~LNetworkAdapterManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LNetworkAdapterManager::GetNetworkCardCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LNetworkAdapterManager::GetNetworkCardName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LNetworkAdapterManager::GetNetworkCardManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LNetworkAdapterManager::GetNetworkCardMACAddress(IN int index, OUT wstring& mac)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"MACAddress", mac);
    }

    bool LNetworkAdapterManager::GetNetworkCardConnectionID(IN int index, OUT wstring& connectionID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"NetConnectionID", connectionID);
    }

    bool LNetworkAdapterManager::GetNetworkCardGUID(IN int index, OUT wstring& guid)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"GUID", guid);
    }

    bool LNetworkAdapterManager::GetNetworkCardPNPDeviceID(IN int index, OUT wstring& pnpDeviceID)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"PNPDeviceID", pnpDeviceID);
    }

    LCDRomDriveManager::LCDRomDriveManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_CDROMDrive");
    }

    LCDRomDriveManager::~LCDRomDriveManager()
    {
        delete m_pWMICoreManager;
        m_pWMICoreManager = 0;
    }

    int LCDRomDriveManager::GetCDRomDriveCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LCDRomDriveManager::GetCDRomDriveName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }
    
}