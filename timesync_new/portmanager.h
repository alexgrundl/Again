#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <vector>
#include <memory>

#include "ThreadWrapper.h"
#include "comm/linux_netport.h"
#include "statemachinemanager.h"

class PortManager
{
public:

    PortManager(INetworkInterfacePort* networkPort, StateMachineManager* stateMachineManager, int portIndex);


    void StartReceiving();

private:

    INetworkInterfacePort* m_networkPort;


    uint32_t Receive(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    std::unique_ptr<CThreadWrapper<PortManager>> m_portThread;


    int m_portIndex;


    StateMachineManager* m_stateMachineManager;
};

#endif // PORTMANAGER_H
