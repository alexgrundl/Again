#include "portmanager.h"

PortManager::PortManager(INetworkInterfacePort* networkPort, StateMachineManager* stateMachineManager, int portIndex)
{
    m_networkPort = networkPort;
    m_stateMachineManager = stateMachineManager;
    m_portIndex = portIndex;

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
        dwWaitResult = pal::EventWait(pWaitHandle, 10);
        if(dwWaitResult == pal::EventWaitTimeout)
        {
            CLinuxReceivePackage package(128);
            m_networkPort->ReceiveMessage(&package);
            if(package.IsValid())
                m_stateMachineManager->ProcessPackage(m_portIndex, &package);
        }
        else
            sleep(1);
    }

    return 0;
}
