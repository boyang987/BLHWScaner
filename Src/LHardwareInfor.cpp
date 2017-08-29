
#include "LHardwareInfor.h"

#include <algorithm>
using std::transform;

#include "Wmi/LWMISystemClasses.h"
#include "Wmi/LWMIHardwareClasses.h"

#include "SetupApi/LSetupAPI.h"

#include "WifiNetwork/LWifiNetwork.h"

#include "DiskController/LDiskController.h"

#include "SMARTPaser/LSMARTPaser.h"

#include "Log/LLog.h"

#include "SMBiosPaser/LSMBiosPaser.h"

#include "Webcam/LWebcam.h"

#include "SystemMetrics/LSystemMetrics.h"

#include "Win8API/Win8API.h"





/// @brief 硬件信息类
///
/// 该类获取的都是固定信息
class CHardwareInfor
{
public:
    /// @brief 获取实例对象
    /// @return 实例对象
    static CHardwareInfor& GetInstance()
    {
        static CHardwareInfor s_instance;
        return s_instance;
    }

    /// @brief 获取计算机系统信息
    /// @return 计算机系统信息
    const ComputerSystemInfor& GetComputerSystemInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanComputerSystemInfor(m_computerSystemInfor);
            s_bScanDone = true;
        }

        return m_computerSystemInfor;
    }

    /// @brief 获取操作系统信息
    /// @return 操作系统信息
    const OperatingSystemInfor& GetOperatingSystemInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanOperatingSystemInfor(m_operatingSystemInfor);
            s_bScanDone = true;
        }

        return m_operatingSystemInfor;
    }

    /// @brief 获取主板信息
    /// @return 主板信息
    const MotherBoardInfor& GetMotherBoardInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanMotherBoardInfor(m_motherBoardInfor);
            s_bScanDone = true;
        }

        return m_motherBoardInfor;
    }

    /// @brief 获取处理器信息
    /// @return 处理器信息
    const ProcessorInfor& GetProcessorInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanProcessorInfor(m_processorInfor);
            s_bScanDone = true;
        }

        return m_processorInfor;
    }

    /// @brief 获取显卡信息
    /// @return 显卡信息
    const VideoCardInforArray& GetVideoCardInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanVideoCardInfor(m_videoCardInfor);
            s_bScanDone = true;
        }

        return m_videoCardInfor;
    }

    /// @brief 获取物理内存信息
    /// @return 物理内存信息
    const PhysicalMemoryInforArray& GetPhysicalMemoryInfor()
    {
        static bool s_bScanDone = false; // 标识是否完成扫描硬件信息
        if (!s_bScanDone)
        {
            this->ScanPhysicalMemoryInfor(m_physicalMemoryInfor);
            s_bScanDone = true;
        }

        return m_physicalMemoryInfor;
    }

    /// @brief 获取磁盘信息
    /// @return 磁盘信息
    const DiskInforArray& GetDiskInfor()
    {
        /*
        磁盘信息会发生变化所以需要在每次获取的时候重新扫描
        */
        this->ScanDiskInfor(m_diskInfor);

        return m_diskInfor;
    }

    /// @brief 获取显示器信息
    /// @return 显示器信息
    const MonitorInforArray& GetMonitorInfor()
    {
        /*
        显示器信息会发生变化所以需要在每次获取的时候重新扫描
        */
        this->ScanMonitorInfor(m_monitorInfor);

        return m_monitorInfor;
    }

    /// @brief 获取电池静态信息
    /// @return 电池静态信息
    const BatteryStaticInfor& GetBatteryStaticInfor()
    {
        this->ScanBatteryStaticInfor(m_batteryStaticInfor);

        return m_batteryStaticInfor;
    }

    /// @brief 获取网卡信息
    /// @return 网卡信息
    const NetworkCardInforArray& GetNetworkCardInfor()
    {
        this->ScanNetworkCardInfor(m_networkCardInfor);

        return m_networkCardInfor;
    }

    /// @brief 获取光驱信息
    /// @return 光驱信息
    const CDRomDriveInforArray& GetCDRomDriveInfor()
    {
        this->ScanCDRomDriveInfor(m_cdRomDriveInfor);

        return m_cdRomDriveInfor;
    }

    /// @brief 获取摄像机信息
    /// @return 摄像机信息
    const CameraInforArray& GetCameraInfor()
    {
        static bool s_bScanDone = false;
        if (!s_bScanDone)
        {
            this->ScanCameraInfor(m_cameraInfor);
            s_bScanDone = true;
        }
        

        return m_cameraInfor;
    }

    /// @brief 获取系统度量信息
    /// @return 系统度量信息
    const SystemMetricsInfor& GetSystemMetricsInfor()
    {
        /*
        系统度量信息会发生变化所以需要在每次获取的时候重新扫描
        */
        this->ScanSystemMetricsInfo(m_systemMetricsInfor);

        return m_systemMetricsInfor;
    }

    const BiometricInforArray& GetBiometricInfo()
    {
        /*
        生物识别设备信息会发生变化所以需要在每次获取的时候重新扫描
        */
        this->ScanBiometricInfo(m_biometricInfor);
        return m_biometricInfor;
    }

    /// @brief 析构函数
    ~CHardwareInfor()
    {

    }

private:
    /// @brief 扫描计算机系统信息
    /// @param[out] computerSystemInfor 存储计算机系统信息
    void ScanComputerSystemInfor(OUT ComputerSystemInfor& computerSystemInfor)
    {
        computerSystemInfor.Type = COMPUTER_TYPE_UNKNOWN;

        LWMI::LComputerSystemManager computerSytemManager;
        computerSytemManager.GetComputerSystemManufacturer(0, computerSystemInfor.Manufacturer);
        LWMI::LComputerSystemManager::LCOMPUTER_SYSTEM_TYPE pcType;
        computerSytemManager.GetComputerSystemPCType(0, pcType);
        computerSystemInfor.Type = (COMPUTER_TYPE)pcType;
        computerSytemManager.GetComputerSystemModel(0, computerSystemInfor.ModelName);

        LWMI::LMSSmBios_RawSMBiosTablesManager smBios;
        vector<unsigned char> data;
        smBios.GetSMBiosData(0, data);
        LSMBiosPaser smBiosPaser(data);
        SMBiosSystemInfor smBiosSystemInfor;
        smBiosPaser.GetSystemInfor(smBiosSystemInfor);
        computerSystemInfor.Version = smBiosSystemInfor.Version;
        computerSystemInfor.SerialNumber = smBiosSystemInfor.SerialNumber;
        for (unsigned int i = 0; i < 16; i++)
        {
            computerSystemInfor.UUID[i] = smBiosSystemInfor.UUID[i];
        }
    }


    /// @brief 扫描操作系统信息
    /// @param[out] operatingSystemInfor 操作系统信息
    void ScanOperatingSystemInfor(OUT OperatingSystemInfor& operatingSystemInfor)
    {
        LWMI::LOperatingSystemManager operatingSystemManager;
        operatingSystemManager.GetOSCaption(0, operatingSystemInfor.Caption);
        operatingSystemManager.GetOSArchitecture(0, operatingSystemInfor.Architecture);
        operatingSystemManager.GetOSVersion(0, operatingSystemInfor.Version);
        operatingSystemManager.GetOSSystemDrive(0, operatingSystemInfor.SystemDrive);
    }

    /// @brief 扫描主板信息
    /// @param[out] motherBoardInfor 主板信息
    void ScanMotherBoardInfor(OUT MotherBoardInfor& motherBoardInfor)
    {
        LWMI::LMSSmBios_RawSMBiosTablesManager smBios;
        vector<unsigned char> data;
        smBios.GetSMBiosData(0, data);
        LSMBiosPaser smBiosPaser(data);

        SMBiosBaseBoardInfor baseBoardInfor;
        smBiosPaser.GetBaseBoardInfor(baseBoardInfor);

        SMBiosBIOSInfor biosInfor;
        smBiosPaser.GetBiosInfor(biosInfor);

        motherBoardInfor.Manufacturer = baseBoardInfor.Manufacturer;
        motherBoardInfor.ProductName = baseBoardInfor.Product;
        motherBoardInfor.Version = baseBoardInfor.Version;
        motherBoardInfor.SerialNumber = baseBoardInfor.SerialNumber;

        motherBoardInfor.BiosVendor = biosInfor.Vendor;
        motherBoardInfor.BiosVersion = biosInfor.Version;
        motherBoardInfor.BiosReleaseDate = biosInfor.ReleaseDate;
        motherBoardInfor.BiosRomSize = biosInfor.RomSize;

        motherBoardInfor.BiosMode = (BIOS_MODE)GetBIOSMode();
    }

    /// @brief 扫描处理器信息
    /// @param[out] processorInfor 存储处理器信息
    void ScanProcessorInfor(OUT ProcessorInfor& processorInfor)
    {
        LWMI::LProcessorManager processorManager;
        processorManager.GetProcessorName(0, processorInfor.Name);
        processorManager.GetProcessorDesription(0, processorInfor.Description);
        processorManager.GetProcessorManufacturer(0, processorInfor.Manufacturer);
        processorManager.GetProcessorCoresNumber(0, processorInfor.CoresNumber);
        processorManager.GetProcessorLogicalProcessorNumber(0, processorInfor.LogicalProcessorNumber);
        processorManager.GetProcessorMaxClockSpeed(0, processorInfor.MaxClockSpeed);
    }

    /// @brief 扫描显卡信息
    /// @param[out] videoCardInfor 存储显卡信息
    void ScanVideoCardInfor(OUT VideoCardInforArray& videoCardInfor)
    {
        videoCardInfor.Count = 0;

        LSetupDisplayCard displayCard;
        int cardCount = displayCard.GetDevNum();
        if (cardCount < 1)
            return;

        videoCardInfor.Count = (unsigned long)cardCount;

        for (int i = 0; i < cardCount && i < MAX_VIDEOCARD_NUMBER; i++)
        {
            videoCardInfor.Type[i] = VIDEO_CARD_UNKNOWN;

            wstring instanceID;
            unsigned long dwRet = displayCard.GetInstanceID(i, instanceID);
            if (0 != dwRet)
                continue;

            videoCardInfor.DevicePath[i] = instanceID;

            unsigned int busNumber;
            dwRet = displayCard.GetBusNumber(i, busNumber);
            if (0 != dwRet)
                continue;

            // 独立显卡挂在PCI插槽口上所以总线号不为0
            // 集成显卡挂在PCI上总线好为0
            if (0 == busNumber)
                videoCardInfor.Type[i] = VIDEO_CARD_INTERNAL;
            else
                videoCardInfor.Type[i] = VIDEO_CARD_EXTERNAL;


            LWMI::LVideoControllerManager videoControllerManager(instanceID);
            if (videoControllerManager.GetVideoControllerCount() != 1)
                continue;

            videoControllerManager.GetVideoControllerDescription(0, videoCardInfor.Description[i]);
            videoControllerManager.GetVideoControllerAdapterRAMCapacity(0, videoCardInfor.RAMSize[i]);
        }

    }

    /// @brief 扫描物理内存信息
    /// @param[out] physicalMemoryInfor 存储物理内存信息
    void ScanPhysicalMemoryInfor(OUT PhysicalMemoryInforArray& physicalMemoryInfor)
    {
        physicalMemoryInfor.Count = 0;

        LWMI::LPhysicalMemoryManager memoryManager;
        physicalMemoryInfor.Count = (unsigned long)memoryManager.GetPhysicalMemoryCount();
        for (int i = 0; i < (int)physicalMemoryInfor.Count && i < MAX_PHYSICAL_MEMORY_NUMBER; i++)
        {
            memoryManager.GetPhysicalMemoryManufacturer(i, physicalMemoryInfor.Manufacturer[i]);
            memoryManager.GetPhysicalMemoryCapacity(i, physicalMemoryInfor.Capacity[i]);
            memoryManager.GetPhysicalMemoryPartNumber(i, physicalMemoryInfor.PartNumbe[i]);
            memoryManager.GetPhysicalMemorySerialNumber(i, physicalMemoryInfor.SerialNumber[i]);
            memoryManager.GetPhysicalMemorySpeed(i, physicalMemoryInfor.Speed[i]);
        }
    }

    /// @brief 扫描磁盘信息
    /// @param[out] diskInfor 存储磁盘信息
    void ScanDiskInfor(OUT DiskInforArray& diskInfor)
    {
        PrintLogW(L"Scan Disk Infor Function");

        diskInfor.Count  = 0;
        LWMI::LDiskDriveManager diskDriveManager;
        diskInfor.Count = (unsigned long)diskDriveManager.GetDiskCount();
        for (int i = 0; i < (int)diskInfor.Count && i < MAX_DISK_NUMBER; i++)
        {
            PrintLogW(L"Disk Index: %d", i);

            diskDriveManager.GetDiskModel(i, diskInfor.Model[i]);
            diskDriveManager.GetDiskSerialNumber(i, diskInfor.SerialNumber[i]);
            diskDriveManager.GetDiskSize(i, diskInfor.TotalSize[i]);
            diskDriveManager.GetDiskLogicalName(i, diskInfor.LogicalName[i]);

            LWMI::LDiskDriveManager::LDISK_TYPE diskType;
            diskDriveManager.GetDiskType(i, diskType);
            diskInfor.DiskType[i] = (DISK_TYPE )diskType;
            diskDriveManager.GetDiskInterfaceType(i, diskInfor.InterfaceType[i]);

            diskInfor.IsATA[i] = false;
            diskInfor.ATAInfor[i].RotationRate = 0;
            diskInfor.ATAInfor[i].SATAType = 0;
            diskInfor.ATAInfor[i].PowerOnHours = 0;

            diskInfor.FixedDiskType[i] = FIXED_DISK_UNKNOWN;

            if (diskInfor.DiskType[i] != FIXED_DISK)
                continue;

            // 获取固定硬盘的其他属性

            wstring pnpDeviceID;
            wstring parentInstanceID;
            diskDriveManager.GetDiskPNPDeviceID(i, pnpDeviceID);
            PrintLogW(L"Disk InstanceID: %s", pnpDeviceID.c_str());
            if (pnpDeviceID.empty())
                continue;

            // 获取磁盘驱动器的实例ID
            LSetupDevAll devAll;
            for (int j = 0; j < devAll.GetDevNum(); j++)
            {
                wstring instanceID;
                devAll.GetInstanceID(j, instanceID);
                if (pnpDeviceID == instanceID)
                {
                    devAll.GetParentInstanceId(j, parentInstanceID);
                    break;
                }
            }

            PrintLogW(L"Disk Controller InstanceID: %s", parentInstanceID.c_str());
            if (parentInstanceID.empty())
                continue;

            // 获取磁盘驱动器的类别和匹配设备ID
            wstring diskControllerClass;
            wstring diskControllerMatchingDeviceID;
            wstring diskControllerDesc;
            for (int j = 0; j < devAll.GetDevNum(); j++)
            {
                wstring instanceID;
                devAll.GetInstanceID(j, instanceID);
                if (parentInstanceID == instanceID)
                {
                    devAll.GetClass(j,diskControllerClass);
                    devAll.GetMatchingDeviceId(j, diskControllerMatchingDeviceID);
                    devAll.GetDevDesc(j, diskControllerDesc);
                    break;
                }
            }

            diskControllerClass = this->WStringToUpper(diskControllerClass);
            diskControllerDesc = this->WStringToUpper(diskControllerDesc);
            diskControllerMatchingDeviceID = this->WStringToUpper(diskControllerMatchingDeviceID);

            PrintLogW(L"Disk Controller Class: %s", diskControllerClass.c_str());
            PrintLogW(L"Disk Controller Matching DeviceID: %s", diskControllerMatchingDeviceID.c_str());
            PrintLogW(L"Disk Controller Desc: %s", diskControllerDesc.c_str());
            if (diskControllerClass.empty())
                continue;
            if (diskControllerDesc.empty())
                continue;

            if (diskControllerClass.compare(L"HDC") == 0) // Hard Disk Controller
            {
                diskInfor.IsATA[i] = true;
                wstring deviceID;
                diskDriveManager.GetDiskDeviceID(i, deviceID);
                LIDEDiskController ideDiskController(deviceID);
                diskInfor.ATAInfor[i].RotationRate = ideDiskController.GetRotationRate();
                diskInfor.ATAInfor[i].SATAType = ideDiskController.GetSATAType();
                if (diskInfor.ATAInfor[i].RotationRate == 1)
                    diskInfor.FixedDiskType[i] = FIXED_DISK_SATA_SSD;
                else
                    diskInfor.FixedDiskType[i] = FIXED_DISK_HDD;

                unsigned char smartData[SMART_DATA_LENGTH] = {0}; // 存储SMART数据
                ideDiskController.GetSMARTData(smartData);
                LSMARTParser smartParser(smartData);
                diskInfor.ATAInfor[i].PowerOnHours = 0;
                smartParser.GetPowerOnHours(diskInfor.ATAInfor[i].PowerOnHours);
            }
            else if (diskControllerClass.compare(L"SCSIADAPTER") == 0) // SCSI and RAID Controllers
            {
                if (diskControllerMatchingDeviceID.compare(L"SD\\CLASS_MMC") == 0)
                    diskInfor.FixedDiskType[i] = FIXED_DISK_EMMC;
                if (diskControllerDesc.find(L"RAID") != wstring::npos)
                    diskInfor.FixedDiskType[i] = FIXED_DISK_RAID;
                if (diskControllerDesc.find(L"NVM EXPRESS") != wstring::npos)
                    diskInfor.FixedDiskType[i] = FIXED_DISK_NVME_SSD;
            }
            else
            {
                diskInfor.FixedDiskType[i] = FIXED_DISK_UNKNOWN;
            }
        }
    }

    /// @brief 扫描显示器信息
    /// @param[out] monitorInfor 存储显示器信息
    void ScanMonitorInfor(OUT MonitorInforArray& monitorInfor)
    {
        monitorInfor.Count = 0;
        LSetupMonitor setupMonitor;
        monitorInfor.Count = (unsigned long)setupMonitor.GetDevNum();
        for (unsigned long i = 0; i < monitorInfor.Count && i < MAX_MONITOR_NUMBER; i++)
        {
            LMonitorExtendInfor extendInfor;
            setupMonitor.GetExtendInfor(i, extendInfor);
            monitorInfor.Name[i] = extendInfor.Name;
            monitorInfor.Date[i] = extendInfor.Date;
        }
    }

    /// @brief 扫描电池静态信息
    /// @param[out] batteryStaticInfor 存储电池静态信息
    void ScanBatteryStaticInfor(OUT BatteryStaticInfor& batteryStaticInfor)
    {
        LWMI::LBatteryManager batteryManager;
        LWMI::LBatteryStaticDataManager batteryStaticDataManager;
        LWMI::LBatteryFullCapacityManager batteryFullCapacityManager;

        if (batteryManager.GetBatteryCount() < 1)
        {
            batteryStaticInfor.Exist = false;
            return;
        }

        batteryStaticInfor.Exist = true;

        batteryManager.GetBatteryName(0, batteryStaticInfor.Name);
        batteryStaticDataManager.GetBatteryManufacturerName(0, batteryStaticInfor.Manufacturer);
        batteryStaticDataManager.GetBatterySerialNumber(0, batteryStaticInfor.SerialNumber);

        batteryStaticDataManager.GetBatteryDesignedCapacity(0, batteryStaticInfor.DesignedCapacity);
        batteryFullCapacityManager.GetBatteryFullChargedCapacity(0, batteryStaticInfor.FullChargedCapacity);
        batteryManager.GetBatteryDesignVoltage(0, batteryStaticInfor.DesignedVoltage);
    }

    /// @brief 扫描网卡信息
    /// @param[out] networkCardInfor 存储网卡信息
    void ScanNetworkCardInfor(OUT NetworkCardInforArray& networkCardInfor)
    {
        // 获取无线网卡GUID
        LWifiNetwork wifiNetwork;
        wstring wlanGUID;
        wifiNetwork.GetWLANGUID(wlanGUID);
        wlanGUID = WStringToUpper(wlanGUID);


        LWMI::LNetworkAdapterManager networkCardManager;
        networkCardInfor.Count = 0;

        for (int i = 0; i < networkCardManager.GetNetworkCardCount() && i < MAX_NETWORKCARD_NUMBER; i++)
        {
            wstring pnpDeviceID;
            networkCardManager.GetNetworkCardPNPDeviceID(i, pnpDeviceID);
            pnpDeviceID = WStringToUpper(pnpDeviceID);

            wstring cardGUID;
            networkCardManager.GetNetworkCardGUID(i, cardGUID);
            cardGUID = WStringToUpper(cardGUID);

            if (wcsncmp(pnpDeviceID.c_str(), L"PCI\\", 4) == 0 ||
                wcsncmp(pnpDeviceID.c_str(), L"USB\\", 4) == 0 ||
                wcsncmp(pnpDeviceID.c_str(), L"SD\\", 3) == 0)
            {
                if (cardGUID.compare(wlanGUID) == 0)
                    networkCardInfor.Type[networkCardInfor.Count] = WIFI_NETCARD;
                else
                    networkCardInfor.Type[networkCardInfor.Count] = ETHERNET_NETCARD;
            }
            else if (wcsncmp(pnpDeviceID.c_str(), L"BTH", 3) == 0)
            {
                networkCardInfor.Type[networkCardInfor.Count] = BLUETOOTH_NETCARD;
            }
            else
            {
                continue;
            }

            networkCardManager.GetNetworkCardName(i, networkCardInfor.Name[networkCardInfor.Count]);
            networkCardManager.GetNetworkCardManufacturer(i, networkCardInfor.Manufacturer[networkCardInfor.Count]);
            networkCardManager.GetNetworkCardMACAddress(i, networkCardInfor.MACAddress[networkCardInfor.Count]);

            networkCardInfor.Count++;
        }
    }

    /// @brief 扫描光驱信息
    /// @param[out] cdRomDrive 存储光驱信息
    void ScanCDRomDriveInfor(OUT CDRomDriveInforArray& cdRomDriveInfor)
    {
        LWMI::LCDRomDriveManager cdRomDriveManager;
        cdRomDriveInfor.Count = (unsigned long)cdRomDriveManager.GetCDRomDriveCount();

        for (int i = 0; i < cdRomDriveManager.GetCDRomDriveCount(); i++)
        {
            cdRomDriveManager.GetCDRomDriveName(i, cdRomDriveInfor.Name[i]);
        }
    }

    /// @brief 扫描摄像机信息
    /// @param[out] cameraInfor 存储摄像机信息
    void ScanCameraInfor(OUT CameraInforArray& cameraInfor)
    {
        PrintLogW(L"Scan Camera Infor Function");
        vector<LWebcam*> webcamVec;
        string error;
        LEnumWebcam(webcamVec, error);

        LSetupImage imageDevice;
        cameraInfor.Count = 0;
        for (int i = 0; i < imageDevice.GetDevNum(); i++)
        {
            wstring instanceId;
            unsigned long lRet = imageDevice.GetInstanceID(i, instanceId);
            PrintLogW(L"Image Device Index: %d, InstanceID: %s", i, instanceId.c_str());
            if (lRet != 0 || instanceId.empty())
                continue;

            for (unsigned int j = 0; j < webcamVec.size(); j++)
            {
                wstring displayName;
                bool bRet = webcamVec[j]->GetDisplayName(displayName);
                PrintLogW(L"Webcam Index: %u, DisplayName: %s", j, displayName.c_str());
                if (!bRet)
                    continue;

                // 先转换为大写, 再替换'#'为'\\'
                displayName = WStringToUpper(displayName);
                for (unsigned int k = 0; k < displayName.size(); k++)
                {
                    if (displayName[k] == L'#')
                    {
                        displayName[k] = L'\\';
                    }
                }
                if (displayName.find(instanceId) == wstring::npos)
                    continue;

                wstring friendlyName;
                webcamVec[j]->GetFriendlyName(friendlyName);

                cameraInfor.Name[cameraInfor.Count] = friendlyName;
                cameraInfor.Count++;
            }
          
        }

        // 释放资源
        for (unsigned int i = 0; i < webcamVec.size(); i++)
        {
            if (webcamVec[i] != 0)
            {
                webcamVec[i]->Release();
                webcamVec[i] = 0;
            }
            
        }
    }

    /// @brief 扫描系统度量信息
    /// @param[out] systemMetricsInfor 存储系统度量信息
    void ScanSystemMetricsInfo(OUT SystemMetricsInfor& systemMetricsInfor)
    {
        systemMetricsInfor.TouchScreenSupported = LSystemMetrics::IsTouchScreenSupported();
    }

    /// @brief 扫描生物识别设备信息
    /// @param[out] biometricInfor 存储生物识别设备信息
    void ScanBiometricInfo(OUT BiometricInforArray& biometricInfor)
    {
        PrintLogW(L"Scan Biometric Infor Function");
        LSetupBiometric biometric;
        biometricInfor.Count = (unsigned long)biometric.GetDevNum();
        for (int i = 0; (i < biometric.GetDevNum()) && (i < MAX_BIOMETRIC_NUMBER); i++)
        {
            PrintLogW(L"Biometric Index: %d", i);
            biometric.GetDevDesc(i, biometricInfor.Description[i]);
            PrintLogW(L"Biometric Desc: %s", biometricInfor.Description[i].c_str());
            biometric.GetManufacturer(i, biometricInfor.Manufacturer[i]);
            PrintLogW(L"Biometric Manufacturer: %s", biometricInfor.Manufacturer[i].c_str());
        }
    }

    /// @brief 将字符串的小写字母转换为大写
    ///  
    /// 不要尝试将非uicode字符串转换大小写, 因为在中文在多字节编码中使用两个字节表示
    /// @param[in] str 原始字符串
    /// @return 转换后的大写字符串
    wstring WStringToUpper(IN const wstring& str)
    {
        wstring newStr = str;
        transform(newStr.begin(), newStr.end(), newStr.begin(), towupper);
        return newStr;
    }

    CHardwareInfor()
    {

    }
    CHardwareInfor(const CHardwareInfor&); // 禁止默认拷贝构造函数
    CHardwareInfor& operator = (const CHardwareInfor&); // 禁止赋值操作符

private:
    ComputerSystemInfor m_computerSystemInfor; ///< 计算机系统信息
    OperatingSystemInfor m_operatingSystemInfor; ///< 操作系统信息
    MotherBoardInfor m_motherBoardInfor; ///< 主板信息
    ProcessorInfor m_processorInfor; ///< 处理器信息
    VideoCardInforArray m_videoCardInfor; ///< 显卡信息
    PhysicalMemoryInforArray m_physicalMemoryInfor; ///< 物理内存信息
    DiskInforArray m_diskInfor; ///< 磁盘信息
    MonitorInforArray m_monitorInfor; ////< 显示器信息
    BatteryStaticInfor m_batteryStaticInfor; ///< 电池静态信息
    NetworkCardInforArray m_networkCardInfor; ///< 网卡信息
    CDRomDriveInforArray m_cdRomDriveInfor; ///< 光驱信息
    CameraInforArray m_cameraInfor; ///< 摄像机信息
    SystemMetricsInfor m_systemMetricsInfor; ///< 系统度量信息
    BiometricInforArray m_biometricInfor; ///< 生物识别设备信息
};


LHardwareInfor::LHardwareInfor()
{

}

LHardwareInfor::~LHardwareInfor()
{

}

const ComputerSystemInfor& LHardwareInfor::GetComputerSystemInfor()
{
    return CHardwareInfor::GetInstance().GetComputerSystemInfor();
}

const OperatingSystemInfor& LHardwareInfor::GetOperatingSystemInfor()
{
    return CHardwareInfor::GetInstance().GetOperatingSystemInfor();
}

const MotherBoardInfor& LHardwareInfor::GetMotherBoardInfor()
{
    return CHardwareInfor::GetInstance().GetMotherBoardInfor();
}

const ProcessorInfor& LHardwareInfor::GetProcessorInfor()
{
    return CHardwareInfor::GetInstance().GetProcessorInfor();
}

const VideoCardInforArray& LHardwareInfor::GetVideoCardInfor()
{
    return CHardwareInfor::GetInstance().GetVideoCardInfor();
}

const PhysicalMemoryInforArray& LHardwareInfor::GetPhysicalMemoryInfor()
{
    return CHardwareInfor::GetInstance().GetPhysicalMemoryInfor();
}

const DiskInforArray& LHardwareInfor::GetDiskInfor()
{
    return CHardwareInfor::GetInstance().GetDiskInfor();
}

const MonitorInforArray& LHardwareInfor::GetMonitorInfor()
{
    return CHardwareInfor::GetInstance().GetMonitorInfor();
}

const BatteryStaticInfor& LHardwareInfor::GetBatteryStaticInfor()
{
    return CHardwareInfor::GetInstance().GetBatteryStaticInfor();
}

const NetworkCardInforArray& LHardwareInfor::GetNetworkCardInfor()
{
    return CHardwareInfor::GetInstance().GetNetworkCardInfor();
}

const CDRomDriveInforArray& LHardwareInfor::GetCDRomDriveInfor()
{
    return CHardwareInfor::GetInstance().GetCDRomDriveInfor();
}

const CameraInforArray& LHardwareInfor::GetCameraInfor()
{
    return CHardwareInfor::GetInstance().GetCameraInfor();
}

const SystemMetricsInfor& LHardwareInfor::GetSystemMetricsInfor()
{
    return CHardwareInfor::GetInstance().GetSystemMetricsInfor();
}

const BiometricInforArray& LHardwareInfor::GetBiometricInfor()
{
    return CHardwareInfor::GetInstance().GetBiometricInfo();
}






















