#ifndef STATEMACHINEMANAGER_H
#define STATEMACHINEMANAGER_H

#include <vector>
#include <memory>

#include "types.h"
#include "ThreadWrapper.h"
#include "statemachinebase.h"
#include "ptpmessagepdelayreq.h"
#include "ptpmessagepdelayresp.h"
#include "ptpmessagepdelayrespfollowup.h"

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
#include "portannouncereceive.h"
#include "portannounceinformation.h"
#include "portroleselection.h"
#include "portannouncetransmit.h"
#include "linkdelaysyncintervalsetting.h"
#ifdef __linux__
    #include "portipc.h"
#endif

#include "ptpclock.h"

class StateMachineManager
{
public:

    StateMachineManager(TimeAwareSystem* timeAwareSystem, std::vector<SystemPort*> ports,
                        std::vector<INetPort*> networkPorts);


    ~StateMachineManager();


    void StartProcessing();


    void ProcessPackage(int portIndex, ReceivePackage *package);

private:

    static const uint32_t m_granularity_ms = 20;


    int m_currentIndexClockUpdate;


    ClockSourceTimeParams clockSourceParams;


    TimeAwareSystem* m_timeAwareSystem;


    std::vector<MDSyncSendSM*> m_mdSyncSendSM;


    std::vector<MDSyncReceiveSM*> m_mdSyncReceiveSM;


    std::vector<PortSyncSyncSend*> m_portSyncSyncSends;


    std::vector<PortSyncSyncReceive*> m_portSyncSyncReceive;


    std::vector<MDPdelayReq*> m_mdPdelayReq;


    std::vector<MDPdelayResp*> m_mdPdelayResp;


    ClockSlaveSync* m_clockSlaveSync;


    SiteSyncSync* m_siteSyncSync;


    ClockMasterSyncSend* m_clockMasterSyncSend;


    ClockMasterSyncReceive* m_clockMasterSyncReceive;


    ClockMasterSyncOffset* m_clockMasterSyncOffset;



    std::vector<PortAnnounceReceive*> m_portAnnounceReceive;


    std::vector<PortAnnounceInformation*> m_portAnnounceInformation;


    PortRoleSelection* m_portRoleSelection;


    std::vector<PortAnnounceTransmit*> m_portAnnounceTransmit;


    std::vector<MDPortAnnounceTransmit*> m_mdPortAnnounceTransmit;


    std::vector<LinkDelaySyncIntervalSetting*> m_linkDelaySyncIntervalSetting;

#ifdef __linux
    std::vector<PortIPC*> m_portIPC;
#endif


    CThreadWrapper<StateMachineManager>* m_stateThread;


    std::vector<StateMachineBase*> m_stateMachines;


    uint32_t Process(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    void InitialProcess();


//    void UpdateTimes();
};

#endif // STATEMACHINEMANAGER_H
