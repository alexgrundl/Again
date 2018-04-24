#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <vector>
#include <memory>

#include "ThreadWrapper.h"
#include "linux_netport.h"
#include "statemachinemanager.h"

class PortManager
{
public:

    PortManager(INetworkInterfacePort* networkPort, std::vector<StateMachineManager*>& stateMachineManagers, int portIndex);


    void StartReceiving();

private:

    INetworkInterfacePort* m_networkPort;


    uint32_t Receive(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    std::unique_ptr<CThreadWrapper<PortManager>> m_portThread;


    int m_portIndex;


    std::vector<StateMachineManager*> m_stateMachineManagers;
};

#endif // PORTMANAGER_H
