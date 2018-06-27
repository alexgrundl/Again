#include "portmanager.h"

PortManager::PortManager(INetPort* networkPort, std::vector<StateMachineManager *> &stateMachineManagers, int portIndex)
{
    m_networkPort = networkPort;
    m_portIndex = portIndex;
    m_receiveUpdGeneralThread = NULL;

    for (std::vector<StateMachineManager*>::size_type i = 0; i < stateMachineManagers.size(); ++i)
    {
        m_stateMachineManagers.push_back(stateMachineManagers[i]);
    }

    m_receiveThread = new CThreadWrapper<PortManager>(this, &PortManager::Receive, std::string("Port receive thread ") +
                                                   std::to_string(portIndex));
    if(networkPort->GetPtpProtocolType() == PTP_PROTOCOL_TYPE_UDP)
    {
        m_receiveUpdGeneralThread = new CThreadWrapper<PortManager>(this, &PortManager::ReceiveUdpGeneral, std::string("Port receive general thread ") +
                                                       std::to_string(portIndex));
    }
}

PortManager::~PortManager()
{
    delete m_receiveThread;
    delete m_receiveUpdGeneralThread;
}

void PortManager::StartReceiving()
{
    m_receiveThread->Start();
    if(m_receiveUpdGeneralThread != NULL)
        m_receiveUpdGeneralThread->Start();
}

void PortManager::StopReceiving()
{
    m_receiveThread->Stop();
    if(m_receiveUpdGeneralThread != NULL)
        m_receiveUpdGeneralThread->Stop();
}

uint32_t PortManager::Receive(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    while(*pbIsRunning)
    {
        if(m_networkPort->IsUpAndConnected())
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
        else
            pal::ms_sleep(1000);
    }

    return 0;
}

uint32_t PortManager::ReceiveUdpGeneral(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    while(*pbIsRunning)
    {
        if(m_networkPort->IsUpAndConnected())
        {
            dwWaitResult = pal::EventWait(pWaitHandle, 20);
            if(dwWaitResult == pal::EventWaitTimeout)
            {
                ReceivePackage package(128);
                m_networkPort->ReceiveGeneralUdpMessage(&package);
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
        else
            pal::ms_sleep(1000);
    }

    return 0;
}
