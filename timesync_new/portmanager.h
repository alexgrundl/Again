#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <vector>
#include <memory>

#include "types.h"
#include "ThreadWrapper.h"
#include "statemachinemanager.h"
#ifdef __linux
#include "linuxnetport.h"
#else
#include "windowsnetport.h"
#endif

class PortManager
{
public:

    PortManager(INetPort* networkPort, std::vector<StateMachineManager*>& stateMachineManagers, int portIndex);


    void StartReceiving();

private:

    INetPort* m_networkPort;


    uint32_t Receive(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    std::unique_ptr<CThreadWrapper<PortManager>> m_portThread;


    int m_portIndex;


    std::vector<StateMachineManager*> m_stateMachineManagers;
};

#endif // PORTMANAGER_H
