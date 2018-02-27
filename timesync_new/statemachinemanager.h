#ifndef STATEMACHINEMANAGER_H
#define STATEMACHINEMANAGER_H

#include <vector>

#include "c-extensions/ThreadWrapper.h"
#include "statemachinebase.h"
#include "ptpmessage/ptpmessagepdelayreq.h"
#include "ptpmessage/ptpmessagepdelayresp.h"
#include "ptpmessage/ptpmessagepdelayrespfollowup.h"

#include "clockmastersyncoffset.h"
#include "clockmastersyncreceive.h"
#include "clockmastersyncsend.h"
#include "clockslavesync.h"
#include "mdpdelayreq.h"
#include "mdpdelayresp.h"
#include "mdsyncreceivesm.h"
#include "mdsyncsendsm.h"
#include "portsyncsyncreceive.h"
#include "portsyncsyncsend.h"
#include "sitesyncsync.h"

class StateMachineManager
{
public:

    StateMachineManager(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGlobal, INetworkInterfacePort *networkPort);


    ~StateMachineManager();


    void StartProcessing();


    void ProcessPackage(IReceivePackage *package);

private:

    static const uint32_t m_granularity_ms = 10;


    TimeAwareSystem* m_timeAwareSystem;

    MDSyncSendSM* m_mdSyncSendSM;


    PortSyncSyncSend* m_portSyncSyncSend;


    ClockSlaveSync* m_clockSlaveSync;


    std::vector<PortSyncSyncSend*> m_portSyncSyncSends;


    SiteSyncSync* m_siteSyncSync;


    PortSyncSyncReceive* m_portSyncSyncReceive;


    MDSyncReceiveSM* m_mdSyncReceiveSM;


    ClockMasterSyncSend* m_clockMasterSyncSend;


    MDPdelayReq* m_mdPdelayReq;


    CThreadWrapper<StateMachineManager>* m_stateThread;


    std::vector<StateMachineBase*> m_stateMachines;


    uint32_t Process(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);

    void InitialProcess();
};

#endif // STATEMACHINEMANAGER_H
