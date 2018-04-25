#include "portmanager.h"

PortManager::PortManager(INetworkInterfacePort* networkPort, std::vector<StateMachineManager *> &stateMachineManagers, int portIndex)
{
    m_networkPort = networkPort;
    m_portIndex = portIndex;

    for (std::vector<StateMachineManager*>::size_type i = 0; i < stateMachineManagers.size(); ++i)
    {
        m_stateMachineManagers.push_back(stateMachineManagers[i]);
    }

    m_portThread = std::unique_ptr<CThreadWrapper<PortManager>>(new CThreadWrapper<PortManager>(this,
        &PortManager::Receive, std::string("Port manager thread ") + std::to_string(portIndex)));
}

void PortManager::StartReceiving()
{
    m_portThread->Start();
}

uint32_t PortManager::Receive(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    while(*pbIsRunning)
    {
        dwWaitResult = pal::EventWait(pWaitHandle, 20);
        if(dwWaitResult == pal::EventWaitTimeout)
        {
            CLinuxReceivePackage package(128);
            m_networkPort->ReceiveMessage(&package);
            if(package.IsValid())
            {

                for (std::vector<StateMachineManager*>::size_type i = 0; i < m_stateMachineManagers.size(); ++i)
                {
                    m_stateMachineManagers[i]->ProcessPackage(m_portIndex, &package);
                }
            }
        }
        else
            sleep(1);
    }

    return 0;
}
