#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <memory>

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
    TimeAwareSystem tas;

    tas.BEGIN = true;
    tas.gmRateRatio = 1.0;
    tas.localTime.ns = 0;
    tas.gmPresent = true;
    tas.clockMasterLogSyncInterval = -3;
    tas.selectedRole.push_back(PORT_ROLE_SLAVE);

    std::vector<std::shared_ptr<INetworkInterfacePort>> networkPorts;
    std::vector<std::shared_ptr<PortGlobal>> ports;

    struct ifaddrs *addrs,*next;
    getifaddrs(&addrs);
    next = addrs;

    while (next)
    {
        if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                (next->ifa_flags & IFF_LOOPBACK) == 0)
        {
            std::shared_ptr<INetworkInterfacePort> networkPort(new NetworkPort(next->ifa_name));
            networkPort->Initialize();
            networkPorts.push_back(networkPort);

            std::shared_ptr<PortGlobal> port(new PortGlobal());
            port->asCapable = true;
            port->portEnabled = true;
            port->pttPortEnabled = true;
            port->thisPort = 1;
            port->syncReceiptTimeout = 3;
            port->computeNeighborPropDelay = true;
            port->computeNeighborRateRatio = true;

            port->pdelayReqInterval.ns = NS_PER_SEC;
            port->pdelayReqInterval.ns_frac = 0;
            port->neighborPropDelayThresh.ns = 900;
            port->neighborPropDelayThresh.ns_frac = 0;

            ports.push_back(port);

            tas.selectedRole.push_back(PORT_ROLE_SLAVE);
            tas.selected.push_back(false);
            tas.reselect.push_back(false);
        }
        next = next->ifa_next;
    }

    StateMachineManager smManager(&tas, ports, networkPorts);
    PortManager portManager(networkPorts, &smManager);
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

