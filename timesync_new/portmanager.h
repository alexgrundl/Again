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

    PortManager(std::vector<std::shared_ptr<INetworkInterfacePort>> networkPorts, StateMachineManager* stateMachineManager);


    void StartReceiving();

private:

    std::vector<std::shared_ptr<INetworkInterfacePort>> m_networkPorts;


    uint32_t Receive(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    CThreadWrapper<PortManager>* m_portThread;


    StateMachineManager* m_stateMachineManager;
};

#endif // PORTMANAGER_H
