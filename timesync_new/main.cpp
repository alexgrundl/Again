#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <memory>
#include <linux/wireless.h>

#include "ptpclock.h"
#include "ptpmessage/ptpmessagesync.h"
#include "ptpmessage/ptpmessagefollowup.h"
#include "ThreadWrapper.h"
#include "statemachinemanager.h"
#include "comm/linux_netport.h"
#include "portmanager.h"

using namespace std;

const char* ifnameMasterClock = "enp15s0";

bool check_wireless(const char* ifname, char* protocol)
{
  int sock = -1;
  struct iwreq pwrq;
  memset(&pwrq, 0, sizeof(pwrq));
  strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return false;
  }

  if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1)
  {
    if (protocol)
        strncpy(protocol, pwrq.u.name, IFNAMSIZ);
    close(sock);
    return true;
  }

  close(sock);
  return false;
}

int main()
{
    std::string ptpMasterClockPath = "/dev/ptp";
    TimeAwareSystem tas;
    std::vector<INetworkInterfacePort*> networkPorts;
    std::vector<PortGlobal*> ports;

    struct ifaddrs *addrs,*next;
    getifaddrs(&addrs);
    next = addrs;

    while (next)
    {
        if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                (next->ifa_flags & IFF_LOOPBACK) == 0 && !check_wireless(next->ifa_name, NULL))
        {
            INetworkInterfacePort* networkPort = new NetworkPort(next->ifa_name);
            networkPort->Initialize();
            networkPorts.push_back(networkPort);

            PortGlobal* port = new PortGlobal();
            port->asCapable = true;
            port->portEnabled = true;
            port->pttPortEnabled = true;
            PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), port->identity.clockIdentity);
            port->identity.portNumber = networkPorts.size();
            port->syncReceiptTimeout = 3;
            port->computeNeighborPropDelay = true;
            port->computeNeighborRateRatio = true;

            port->pdelayReqInterval.ns = NS_PER_SEC;
            port->pdelayReqInterval.ns_frac = 0;
            port->neighborPropDelayThresh.ns = 900;
            port->neighborPropDelayThresh.ns_frac = 0;

            port->rcvdMsg = false;
            port->updtInfo = true;
            port->announceReceiptTimeout = 3;
            port->announceInterval.ns = NS_PER_SEC;

            port->portPriority.identity.priority1 = 255;
            port->portPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
            port->portPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
            port->portPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
            port->portPriority.identity.priority2 = 255;
            memset(port->portPriority.identity.clockIdentity, 255, sizeof(port->portPriority.identity.clockIdentity));
            port->portPriority.stepsRemoved = UINT16_MAX;
            memset(port->portPriority.sourcePortIdentity.clockIdentity, 255, sizeof(port->portPriority.sourcePortIdentity.clockIdentity));
            port->portPriority.sourcePortIdentity.portNumber = UINT16_MAX;
            port->portPriority.portNumber = UINT16_MAX;

            ports.push_back(port);

            tas.selectedRole.push_back(PORT_ROLE_SLAVE);
            tas.selected.push_back(false);
            tas.reselect.push_back(false);

            printf("Name: %s\n", next->ifa_name);

            if(strcmp(ifnameMasterClock, next->ifa_name) == 0)
            {
                PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), tas.thisClock);
                ptpMasterClockPath += std::to_string(((NetworkPort*)networkPort)->GetPtpClockIndex());
            }
        }
        next = next->ifa_next;
    }
    freeifaddrs(addrs);


    StateMachineManager smManager(&tas, ports, networkPorts, ptpMasterClockPath);
    smManager.StartProcessing();

    std::vector<PortManager*> portManagers;
    for (std::vector<INetworkInterfacePort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
            portManagers.push_back(new PortManager(networkPorts[i], &smManager, i));
            portManagers[i]->StartReceiving();
    }

    while(true)
    {
        usleep(1000 * 10);
    }

    return 0;
}

