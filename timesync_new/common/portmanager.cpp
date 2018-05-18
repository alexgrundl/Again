#include "portmanager.h"

PortManager::PortManager(INetPort* networkPort, std::vector<StateMachineManager *> &stateMachineManagers, int portIndex)
{
    m_networkPort = networkPort;
    m_portIndex = portIndex;

    for (std::vector<StateMachineManager*>::size_type i = 0; i < stateMachineManagers.size(); ++i)
    {
        m_stateMachineManagers.push_back(stateMachineManagers[i]);
    }

    m_portThread = new CThreadWrapper<PortManager>(this, &PortManager::Receive, std::string("Port manager thread ") +
                                                   std::to_string(portIndex));
}

PortManager::~PortManager()
{
    delete m_portThread;
}

void PortManager::StartReceiving()
{
    m_portThread->Start();
}

void PortManager::StopReceiving()
{
    m_portThread->Stop();
}

uint32_t PortManager::Receive(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    while(*pbIsRunning)
    {
        dwWaitResult = pal::EventWait(pWaitHandle, 20);
        if(dwWaitResult == pal::EventWaitTimeout)
        {
            ReceivePackage package(128);
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
        {
            pal::ms_sleep(10);
        }
    }

    return 0;
}
