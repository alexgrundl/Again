#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <memory>
#include <linux/wireless.h>
#include <signal.h>

#include "ptpclock.h"
#include "ptpmessagesync.h"
#include "ptpmessagefollowup.h"
#include "ThreadWrapper.h"
#include "statemachinemanager.h"
#include "linux_netport.h"
#include "portmanager.h"

using namespace std;

const char* ifnameMasterClock = "enp15s0";

void wait_for_signals()
{
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
    {
        perror("pthread_sigmask()");
        return;
    }

    do {
        sig = 0;

        if (sigwait(&set, &sig) != 0) {
            perror("sigwait()");
            return;
        }

    } while (sig != SIGINT);
}

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
    TimeAwareSystem tas;
    TimeAwareSystem tas1;

    std::vector<INetworkInterfacePort*> networkPorts;
    std::vector<PortGlobal*> ports;
    std::vector<PortGlobal*> ports1;

    struct ifaddrs *addrs,*next;
    getifaddrs(&addrs);
    next = addrs;

    while (next)
    {
        if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                (next->ifa_flags & IFF_LOOPBACK) == 0 && !check_wireless(next->ifa_name, NULL))
        {
            if(strcmp(next->ifa_name, "enp15s0") == 0)
            {
                INetworkInterfacePort* networkPort = new NetworkPort(next->ifa_name);
                networkPort->Initialize();
                networkPorts.push_back(networkPort);

                for (int i = 0; i < 2; ++i)
                {
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

                    if(i == 0)
                    {
                        ports.push_back(port);

                        tas.AddSelectedRole(PORT_ROLE_SLAVE);
                        tas.selected.push_back(false);
                        tas.reselect.push_back(false);

                        printf("Name: %s\n", next->ifa_name);

                        if(strcmp(ifnameMasterClock, next->ifa_name) == 0)
                        {
                            uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                            PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC);
                            tas.SetClockIdentity(clockIdentityFromMAC);
                            tas.InitLocalClock(((NetworkPort*)networkPort)->GetPtpClockIndex());
                        }
                    }
                    else
                    {
                        ports1.push_back(port);

                        tas1.AddSelectedRole(PORT_ROLE_SLAVE);
                        tas1.selected.push_back(false);
                        tas1.reselect.push_back(false);
                        tas1.SetDomain(1);

                        printf("Name: %s\n", next->ifa_name);

                        if(strcmp(ifnameMasterClock, next->ifa_name) == 0)
                        {
                            uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                            PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC);
                            tas1.SetClockIdentity(clockIdentityFromMAC);
                            tas1.InitLocalClock(((NetworkPort*)networkPort)->GetPtpClockIndex());
                        }
                    }
                }
            }
        }
        next = next->ifa_next;
    }
    freeifaddrs(addrs);

    std::vector<StateMachineManager*> smManagers;

    StateMachineManager smManager(&tas, ports, networkPorts);
    smManagers.push_back(&smManager);
    StateMachineManager smManager1(&tas1, ports1, networkPorts);
    smManagers.push_back(&smManager1);
    smManager.StartProcessing();
    smManager1.StartProcessing();

    std::vector<PortManager*> portManagers;
    for (std::vector<INetworkInterfacePort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
            portManagers.push_back(new PortManager(networkPorts[i], smManagers, i));
            portManagers[i]->StartReceiving();
    }

    wait_for_signals();

    return 0;
}

