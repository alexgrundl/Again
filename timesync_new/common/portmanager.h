#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <vector>
#include <memory>

#include "types.h"
#include "ThreadWrapper.h"
#include "statemachinemanager.h"
#ifdef __linux
#include "netportlinux.h"
#else
#include "netportwindows.h"
#endif

class PortManager
{
public:

    PortManager(INetPort* networkPort, std::vector<StateMachineManager*>& stateMachineManagers, int portIndex);


    virtual ~PortManager();


    void StartReceiving();


    void StopReceiving();

private:

    INetPort* m_networkPort;


    uint32_t Receive(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    CThreadWrapper<PortManager>* m_portThread;


    int m_portIndex;


    std::vector<StateMachineManager*> m_stateMachineManagers;
};

#endif // PORTMANAGER_H
