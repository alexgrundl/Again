#ifndef STATEMACHINEMANAGER_H
#define STATEMACHINEMANAGER_H

#include <vector>
#include <memory>

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
#include "portannouncereceive.h"
#include "portannounceinformation.h"
#include "portroleselection.h"

#include "ptpclock.h"

class StateMachineManager
{
public:

    StateMachineManager(TimeAwareSystem* timeAwareSystem, std::vector<std::shared_ptr<PortGlobal>> ports,
                        std::vector<std::shared_ptr<INetworkInterfacePort>> networkPorts);


    ~StateMachineManager();


    void StartProcessing();


    void ProcessPackage(int portIndex, IReceivePackage *package);

private:

    static const uint32_t m_granularity_ms = 10;


    static const uint32_t m_clockSourceTimeUpdate = 10;


    int m_currentIndexClockUpdate;


    ClockSourceTimeParams clockSourceParams;


    TimeAwareSystem* m_timeAwareSystem;


    std::vector<std::shared_ptr<MDSyncSendSM>> m_mdSyncSendSM;


    std::vector<std::shared_ptr<MDSyncReceiveSM>> m_mdSyncReceiveSM;


    std::vector<std::shared_ptr<PortSyncSyncSend>> m_portSyncSyncSends;


    std::vector<std::shared_ptr<PortSyncSyncReceive>> m_portSyncSyncReceive;


    std::vector<std::shared_ptr<MDPdelayReq>> m_mdPdelayReq;


    std::vector<std::shared_ptr<MDPdelayResp>> m_mdPdelayResp;


    std::shared_ptr<ClockSlaveSync> m_clockSlaveSync;


    std::shared_ptr<SiteSyncSync> m_siteSyncSync;


    std::shared_ptr<ClockMasterSyncSend> m_clockMasterSyncSend;


    std::shared_ptr<ClockMasterSyncReceive> m_clockMasterSyncReceive;


    std::shared_ptr<ClockMasterSyncOffset> m_clockMasterSyncOffset;



    std::vector<std::shared_ptr<PortAnnounceReceive>> m_portAnnounceReceive;


    std::vector<std::shared_ptr<PortAnnounceInformation>> m_portAnnounceInformation;


    std::shared_ptr<PortRoleSelection> m_portRoleSelection;



    CThreadWrapper<StateMachineManager>* m_stateThread;


    std::vector<std::shared_ptr<StateMachineBase>> m_stateMachines;


    uint32_t Process(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);

    void InitialProcess();


    void UpdateTimes();
};

#endif // STATEMACHINEMANAGER_H
