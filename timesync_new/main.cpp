#include <iostream>
#include <sys/types.h>
#include <memory>
#include <signal.h>

#include "types.h"

#ifdef __linux__
#include "licensechecklinux.h"
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include "ptpclocklinux.h"
#else
#include <io.h>
#include "ptpclockwindows.h"
#include "licensecheckwindows.h"
#endif


#include "ptpmessagesync.h"
#include "ptpmessagefollowup.h"
#include "ThreadWrapper.h"
#include "statemachinemanager.h"
#include "linuxnetport.h"
#include "portmanager.h"

using namespace std;

#ifdef __arm__
    #ifndef NO_LICENSE_CHECK
    #define NO_LICENSE_CHECK
    #endif
#endif

void wait_for_signals()
{
#ifdef __linux__

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

#else
    while(true)
        pal::ms_sleep(1000);
#endif
}

bool check_wireless(const char* ifname, char* protocol)
{
#ifdef __linux__
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
#endif
  return false;
}

int main()
{
    LicenseCheck* licenseCheck;
    bool checkLicense = true;

#ifndef __linux__
    licenseCheck = new LicenseCheckWindows();
#else
    licenseCheck = new LicenseCheckLinux();
#endif

#ifdef NO_LICENSE_CHECK
    checkLicense = false;
#endif

    if(!checkLicense || licenseCheck->LicenseValid())
    {
        TimeAwareSystem tas;
        TimeAwareSystem tas1;

    std::vector<INetPort*> networkPorts;
        std::vector<PortGlobal*> ports;
        std::vector<PortGlobal*> ports1;

    #ifndef __linux__
        PtpClock* clockDom0 = new PtpClockWindows();
        PtpClock* clockDom1 = new PtpClockWindows();
    #else
        PtpClock* clockDom0 = new PtpClockLinux();
        PtpClock* clockDom1 = new PtpClockLinux();

        char macLicense[ETH_ALEN], ifnameLicense[IFNAMSIZ];
        struct ifaddrs *addrs,*next;
        getifaddrs(&addrs);
        next = addrs;

        ((LicenseCheckLinux*)licenseCheck)->GetMacOfInterfaceWithLicense(macLicense, ifnameLicense);
        while (next)
        {
            if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                    (next->ifa_flags & IFF_LOOPBACK) == 0 && !check_wireless(next->ifa_name, NULL))
            {
    //            if(strcmp(next->ifa_name, "enp15s0") == 0 || strcmp(next->ifa_name, "enp3s0f0") == 0)
    //            {
                    INetworkInterfacePort* networkPort = new NetworkPort(next->ifa_name);
                    networkPort->Initialize();
                    networkPorts.push_back(networkPort);

                    for (int i = 0; i < 2; ++i)
                    {
                        PortGlobal* port = new PortGlobal();
                        PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), port->identity.clockIdentity);
                        port->identity.portNumber = networkPorts.size();

                        if(i == 0)
                        {
                            ports.push_back(port);

                            tas.AddSelectedRole(PORT_ROLE_SLAVE);

                            lognotice("Sending on interface: %s", next->ifa_name);

                            if(strcmp(ifnameLicense, next->ifa_name) == 0)
                            {
                                uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                                PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC);
                                tas.SetClockIdentity(clockIdentityFromMAC);
                                tas.InitLocalClock(clockDom0, ((NetworkPort*)networkPort)->GetPtpClockIndex());
                            }
                        }
                        else
                        {
                            ports1.push_back(port);

                            tas1.AddSelectedRole(PORT_ROLE_SLAVE);
                            tas1.SetDomain(1);

                            if(strcmp(ifnameLicense, next->ifa_name) == 0)
                            {
                                uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                                PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC);
                                tas1.SetClockIdentity(clockIdentityFromMAC);
                                tas1.InitLocalClock(clockDom1, ((NetworkPort*)networkPort)->GetPtpClockIndex());
                            }
                        }
                    }
    //            }
            }
            next = next->ifa_next;
        }
        freeifaddrs(addrs);
    #endif

        std::vector<StateMachineManager*> smManagers;

        StateMachineManager smManager(&tas, ports, networkPorts);
        smManagers.push_back(&smManager);
        StateMachineManager smManager1(&tas1, ports1, networkPorts);
        smManagers.push_back(&smManager1);
        smManager.StartProcessing();
        smManager1.StartProcessing();

        std::vector<PortManager*> portManagers;
    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
        {
                portManagers.push_back(new PortManager(networkPorts[i], smManagers, i));
                portManagers[i]->StartReceiving();
        }

        wait_for_signals();

        clockDom0->StopPPS();
        delete clockDom0;
        delete clockDom1;
    }

    delete licenseCheck;
    return 0;
}

