#include "portmanager.h"

PortManager::PortManager(std::vector<std::shared_ptr<INetworkInterfacePort>> networkPorts, StateMachineManager* stateMachineManager)
{
    m_networkPorts = networkPorts;
    m_stateMachineManager = stateMachineManager;

    for (std::vector<std::shared_ptr<INetworkInterfacePort>>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_portThread = new CThreadWrapper<PortManager>(this, &PortManager::Receive,
                                                       std::string("Port manager thread ") + std::to_string(i + 1));
    }
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
        dwWaitResult = pal::EventWait(pWaitHandle, 10);
        if(dwWaitResult == pal::EventWaitTimeout)
        {
            for (std::vector<NetworkPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
            {
                CLinuxReceivePackage package(128);
                m_networkPorts[i]->ReceiveMessage(&package);
                m_stateMachineManager->ProcessPackage(i, &package);
            }
        }
    }

    return 0;
}
