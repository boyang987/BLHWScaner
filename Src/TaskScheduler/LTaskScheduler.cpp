
#include "LTaskScheduler.h"

#include <Windows.h>
#include <Taskschd.h>
#include <comdef.h>


#pragma comment(lib, "Taskschd.lib")

/// @brief COM初始化类
/// 只支持单线程
class CInitCom
{
public:
    CInitCom()
    {
        this->bSuccess = false;

        // CoInitialize调用后返回S_FALSE或S_OK后都需调用CoUninitialize
        // CoInitialize调用后返回RPC_E_CHANGE_MODE, 表明当前线程已被初始化(且和当前模式不同),不需要调用CoUninitialize
        HRESULT hr = CoInitialize(NULL);
        if (hr == S_OK || hr == S_FALSE)
        {
            this->bSuccess = true;
        }
        else
        {
            this->bSuccess = false;
        }
    }

    ~CInitCom()
    {
        if (this->bSuccess)
        {
            CoUninitialize();
        }
    }
private:
    bool bSuccess; ///< 标识是否初始化成功
};

/// @brief 任务计划类
class CTaskScheduler
{
public:
    /// @brief 删除一个任务计划
    /// @param[in] pFolderName 任务计划所在的文件夹名称, 如果为NULL, 则表示任务计划在根目录下
    /// @param[in] pTaskName 任务计划名称
    /// @return 成功返回true, 失败返回false
    static bool Delete(IN const wchar_t* pFolderName, const wchar_t* pTaskName)
    {
        bool bRet = true;
        CInitCom initCom;
        ITaskService* pTaskService = NULL;
        ITaskFolder* pRootFolder = NULL;
        ITaskFolder* pMyFolder = NULL;

        if (NULL == pTaskName)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        HRESULT hr = CoCreateInstance( 
            CLSID_TaskScheduler,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_ITaskService,
            (void**)&pTaskService);
        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        hr = pTaskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 获取根文件夹
        hr = pTaskService->GetFolder( _bstr_t( L"\\"), &pRootFolder);
        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (NULL != pFolderName)
        {
            hr = pRootFolder->GetFolder(_bstr_t(pFolderName), &pMyFolder);
            if(FAILED(hr))
            {
                bRet = false;
                goto SAFE_EXIT;
            }
        }

        if (NULL != pMyFolder)
        {
            hr = pMyFolder->DeleteTask(_bstr_t(pTaskName), 0); 
        }
        else
        {
            hr = pRootFolder->DeleteTask(_bstr_t(pTaskName), 0);
        }

        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        bRet = true;

SAFE_EXIT:
        if (NULL != pTaskService)
        {
            pTaskService->Release();
            pTaskService = NULL;
        }

        if (NULL != pRootFolder)
        {
            pRootFolder->Release();
            pRootFolder = NULL;
        }
        if (NULL != pMyFolder)
        {
            pMyFolder->Release();
            pMyFolder = NULL;
        }

        return bRet;
    }

public:
    /// @brief 构造函数
    CTaskScheduler()
    {
        m_pRootTaskFolder = NULL;
        m_pTaskDef = NULL;

        ITaskService* pTaskService = NULL;
        

        HRESULT hr = CoCreateInstance( 
            CLSID_TaskScheduler,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_ITaskService,
            (void**)&pTaskService);
        if (FAILED(hr))
        {
            goto SAFE_EXIT;
        }


        hr = pTaskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
        if (FAILED(hr))
        {
            goto SAFE_EXIT;
        }

        // 获取根文件夹
        hr = pTaskService->GetFolder( _bstr_t( L"\\"), &m_pRootTaskFolder);
        if (FAILED(hr))
        {
            goto SAFE_EXIT;
        }

        // 创建任务
        hr = pTaskService->NewTask(0, &m_pTaskDef);
        if (FAILED(hr))
        {
            goto SAFE_EXIT;
        }

        return;

SAFE_EXIT:
        if (NULL != pTaskService)
        {
            pTaskService->Release();
            pTaskService = NULL;
        }

    }

    /// @brief 析构函数
    ~CTaskScheduler()
    {
        if (NULL != m_pTaskDef)
        {
            m_pTaskDef->Release();
            m_pTaskDef = NULL;
        }

        if (NULL != m_pRootTaskFolder)
        {
            m_pRootTaskFolder->Release();
            m_pRootTaskFolder = NULL;
        }
    }

    /// @brief 设置任务主要属性
    bool Principal(IN bool bHighestLevel)
    {
        if (NULL == m_pTaskDef)
        {
            return false;
        }


        bool bRet = true;
        IPrincipal* pPrincipal = NULL;

        HRESULT hr = m_pTaskDef->get_Principal(&pPrincipal);
        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (bHighestLevel)
            hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
        else
            hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_LUA);

        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

SAFE_EXIT:

        if (NULL != pPrincipal)
        {
            pPrincipal->Release();
            pPrincipal = NULL;
        }

        return bRet;
    }

    /// @brief 设置触发条件
    /// @param[in] trigger, 1(启动时触发), 2(登录时触发)
    /// @return 成功返回true, 失败返回false
    bool Trigger(IN unsigned int trigger)
    {
        if (trigger != 1 && trigger != 2)
            return false;

        if (NULL == m_pTaskDef)
            return false;

        bool bRet = true;

        ITriggerCollection* pTriggerCollection = NULL;
        ITrigger* pTrigger = NULL;
        //得到触发器集合    
        HRESULT hr = m_pTaskDef->get_Triggers(&pTriggerCollection);
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        //在触发器集合中创建触发器
        if (1 == trigger)
            hr = pTriggerCollection->Create(TASK_TRIGGER_BOOT, &pTrigger); // 系统启动时触发
        else if (2 == trigger)
            hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger); // 当用户登录时触发

        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

SAFE_EXIT:
        if (NULL != pTriggerCollection)
        {
            pTriggerCollection->Release();
            pTriggerCollection = NULL;
        }
        if (NULL != pTrigger)
        {
            pTrigger->Release();
            pTrigger = NULL;
        }

        return bRet;
    }

    /// @brief 设置设置项
    bool Settings(IN bool bStartOnBattery)
    {
        if (NULL == m_pTaskDef)
            return false;

        bool bRet = true;
        ITaskSettings* pSettings = NULL;
        HRESULT hr = m_pTaskDef->get_Settings(&pSettings);
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_BOOL(!bStartOnBattery));
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        bRet = true;

SAFE_EXIT:
        if (NULL != pSettings)
        {
            pSettings->Release();
            pSettings = NULL;
        }

        return bRet;
    }

    /// @brief 设置任务计划动作
    bool Action(IN const wchar_t* pExePath, IN const wchar_t* pParam, IN const wchar_t* pWorkingDir)
    {
        if (NULL == pExePath)
            return false;
        if (NULL == m_pTaskDef)
            return false;

        bool bRet = true;

        IActionCollection* pActionCollection = NULL;
        IAction* pAction = NULL;
        IExecAction* pExecAction = NULL;

        //得到动作集合    
        HRESULT hr = m_pTaskDef->get_Actions(&pActionCollection);
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        //在动作集合中创建动作    
        hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        //向动作里面写入执行程序
        hr = pAction->QueryInterface( IID_IExecAction, (void**)&pExecAction );
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        hr = pExecAction->put_Path(_bstr_t(pExePath)); //运行本程序
        if(FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (NULL != pParam)
        {
            hr = pExecAction->put_Arguments(_bstr_t(pParam));
            if(FAILED(hr))
            {
                bRet = false;
                goto SAFE_EXIT;
            }
        }
        if (NULL != pWorkingDir)
        {
            hr = pExecAction->put_WorkingDirectory(_bstr_t(pWorkingDir));
            if (FAILED(hr))
            {
                bRet = false;
                goto SAFE_EXIT;
            }
        }
        

        bRet = true;

SAFE_EXIT:
        if (NULL != pExecAction)
        {
            pExecAction->Release();
            pExecAction = NULL;
        }
        if (NULL != pActionCollection)
        {
            pActionCollection->Release();
            pActionCollection = NULL;
        }
        if (NULL != pAction)
        {
            pAction->Release();
            pAction = NULL;
        }

        return bRet;

    }

    /// @brief 使用任务计划名称创建新的任务计划
    bool Register(IN const wchar_t* pFolderName, IN const wchar_t* pTaskName)
    {
        if (NULL == pTaskName)
            return false;
        if (NULL == m_pTaskDef)
            return false;
        if (NULL == m_pRootTaskFolder)
            return false;

        bool bRet = true;
        HRESULT hr = S_OK;
        ITaskFolder* pNewFolder = NULL;
        IRegisteredTask* pRegisteredTask = NULL;
        ITaskFolder* pMyFolder = NULL;

        if (NULL != pFolderName)
        {
            hr = m_pRootTaskFolder->GetFolder(_bstr_t(pFolderName), &pNewFolder);
            if(FAILED(hr)) //如果文件夹不存在,就创建一个
            {
                hr = m_pRootTaskFolder->CreateFolder(_bstr_t(pFolderName), _variant_t(), &pNewFolder);
                if(FAILED(hr))
                {
                    bRet = false;
                    goto SAFE_EXIT;
                }
            }
        }
        
        // 如果指定了文件夹, 则在指定的文件夹下创建任务计划, 否则在根目录下创建任务计划
        if (NULL != pNewFolder)
        {
            pMyFolder = pNewFolder;
        }
        else
        {
            pMyFolder = m_pRootTaskFolder;
        }
        
        hr = pMyFolder->RegisterTaskDefinition(
            _bstr_t(pTaskName),
            m_pTaskDef,
            TASK_CREATE_OR_UPDATE,  
            _variant_t(),
            _variant_t(),
            TASK_LOGON_INTERACTIVE_TOKEN,
            _variant_t(L""),
            &pRegisteredTask);
        if (FAILED(hr))
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

SAFE_EXIT:
        if (NULL != pNewFolder)
        {
            pNewFolder->Release();
            pNewFolder = NULL;
        }
        if (NULL != pRegisteredTask)
        {
            pRegisteredTask->Release();
            pRegisteredTask = NULL;
        }

        return bRet;

    }

private:

    ITaskDefinition* m_pTaskDef; ///< 任务计划定义对象
    ITaskFolder* m_pRootTaskFolder; ///< 任务计划根文件夹对象
    CInitCom m_initCom; ///< 初始化COM对象
};

bool LTaskScheduler::Delete(IN const wchar_t* pFolderName, const wchar_t* pTaskName)
{
    return CTaskScheduler::Delete(pFolderName, pTaskName);
}

LTaskScheduler::LTaskScheduler()
{
    m_pTaskScheduler = new CTaskScheduler();
}

LTaskScheduler::~LTaskScheduler()
{
    if (NULL != m_pTaskScheduler)
    {
        delete m_pTaskScheduler;
    }
}

bool LTaskScheduler::Principal(IN bool bHighestLevel)
{
    if (NULL == m_pTaskScheduler)
        return false;

    return m_pTaskScheduler->Principal(bHighestLevel);
}

bool LTaskScheduler::Trigger(IN unsigned int trigger)
{
    if (NULL == m_pTaskScheduler)
        return false;

    return m_pTaskScheduler->Trigger(trigger);
}

bool LTaskScheduler::Settings(IN bool bStartOnBattery)
{
    if (NULL == m_pTaskScheduler)
        return false;

    return m_pTaskScheduler->Settings(bStartOnBattery);
}

bool LTaskScheduler::Action(IN const wchar_t* pExePath, IN const wchar_t* pParam, IN const wchar_t* pWorkingDir)
{
    if (NULL == m_pTaskScheduler)
        return false;

    return m_pTaskScheduler->Action(pExePath, pParam, pWorkingDir);
}

bool LTaskScheduler::Register(IN const wchar_t* pFolderName, IN const wchar_t* pTaskName)
{
    if (NULL == m_pTaskScheduler)
        return false;

    return m_pTaskScheduler->Register(pFolderName, pTaskName);
}