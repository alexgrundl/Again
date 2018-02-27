#include <iostream>
#include <unistd.h>

#include "clockmastersyncoffset.h"
#include "clockmastersyncreceive.h"
#include "clockmastersyncsend.h"
#include "clockslavesync.h"
#include "portsyncsyncsend.h"
#include "portsyncsyncreceive.h"
#include "sitesyncsync.h"
#include "ptpclock.h"
#include "mdsyncsendsm.h"
#include "mdsyncreceivesm.h"
#include "ptpmessage/ptpmessagesync.h"
#include "ptpmessage/ptpmessagefollowup.h"
#include "c-extensions/ThreadWrapper.h"
#include "statemachinemanager.h"
#include "mdpdelayreq.h"
#include "comm/linux_netport.h"
#include "portmanager.h"

using namespace std;

int main()
{
//    TimeAwareSystem tas;
//    UScaledNs localTimeOld;
//    ClockSourceTimeParams params, params_old;

//    params.sourceTime.sec = 400;
//    params.sourceTime.ns = 5000;
//    params.sourceTime.ns_frac = 0;

//    params_old.sourceTime.sec = 200;
//    params_old.sourceTime.ns = 2500;
//    params_old.sourceTime.ns_frac = 0;

//    tas.localTime.ns = (uint64_t)200 * 1000 * 1000 * 1000;
//    tas.localTime.ns_frac = 0;
//    localTimeOld.ns = 0;
//    localTimeOld.ns_frac = 0;

//    tas.gmRateRatio = (params.sourceTime - params_old.sourceTime) / (tas.localTime - localTimeOld);

//    printf("Sizeof uint64_t: %lu\n", sizeof(uint64_t));
//    printf("%f\n", tas.gmRateRatio);




    TimeAwareSystem tas;
    NetworkPort networkPort("enx74da384a1b56");
    std::vector<NetworkPort*> networtPorts;

    networkPort.Initialize();
    networtPorts.push_back(&networkPort);
//    PtpClock clock;
//    ClockSourceTimeParams params;

//    ClockMasterSyncReceive cmSyncRecv;

    tas.BEGIN = true;
    tas.localClockTickInterval.ns = (uint64_t)500 * 1000 * 1000;
    tas.localClockTickInterval.ns_frac = 0;
    tas.gmRateRatio = 1.0;
    tas.localTime.ns = 0;
    tas.gmPresent = true;
//    tas.localTime.ns_frac = 0;
//    cmSyncRecv.SetTimeAwareSystem(&tas);
//    cmSyncRecv.ProcessState();
//    tas.BEGIN = false;

//    int i = 10;
//    while(true)
//    {
//        usleep(tas.localClockTickInterval.ns / 1000);
//        if(i == 10)
//        {
//            clock.Invoke(&params);
//            cmSyncRecv.SetClockSourceRequest(&params);
//            i = 1;
//        }
//        else
//        {
//            cmSyncRecv.SignalLocalClockUpdate();
//            i++;
//        }
//    }

    PortGlobal port, port2;
    port.asCapable = true;
    port.portEnabled = true;
    port.pttPortEnabled = true;
    port.thisPort = 1;

    port2.asCapable = true;
    port2.portEnabled = true;
    port2.pttPortEnabled = true;
    port2.thisPort = 1;


    std::vector<PortGlobal> ports;
    ports.push_back(port);
    ports.push_back(port2);
    tas.selectedRole.push_back(PORT_ROLE_SLAVE);
    tas.selectedRole.push_back(PORT_ROLE_MASTER);

    MDGlobal mdGlobal;
    mdGlobal.pdelayReqInterval.ns = NS_PER_SEC;
    mdGlobal.pdelayReqInterval.ns_frac = 0;

    PtpMessageSync messageSync;
    messageSync.SetDomainNumber(0);
    messageSync.SetLogMessageInterval(-3);
    messageSync.SetSequenceID(30000);


    PtpMessageFollowUp messageFollowUp;
    messageFollowUp.SetDomainNumber(0);
    messageFollowUp.SetLogMessageInterval(-3);
    messageFollowUp.SetSequenceID(30000);

    StateMachineManager smManager(&tas, &port, &mdGlobal, &networkPort);
    PortManager portManager(networtPorts, &smManager);
    smManager.StartProcessing();
    portManager.StartReceiving();

    while(true)
    {
//        mdSyncReceiveSM.SetSyncMessage(&messageSync);
        usleep(1000 * 10);
//        mdSyncReceiveSM.SetFollowUpMessage(&messageFollowUp);
    }

    return 0;
}

