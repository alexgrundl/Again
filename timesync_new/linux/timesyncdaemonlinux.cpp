#ifdef __linux__

#include "timesyncdaemonlinux.h"

TimeSyncDaemonLinux::TimeSyncDaemonLinux(int nDomains) : TimeSyncDaemon(nDomains)
{
    m_licenseCheck = new LicenseCheckLinux();
}

TimeSyncDaemonLinux::~TimeSyncDaemonLinux()
{
    delete m_licenseCheck;
    DeleteManagers();
    DeleteNetworkPorts();
}

void TimeSyncDaemonLinux::InitalizePorts()
{
    char macLicense[ETH_ALEN], ifnameLicense[IFNAMSIZ];
    struct ifaddrs *addrs,*next;

    DeleteNetworkPorts();

    getifaddrs(&addrs);
    next = addrs;

    if(m_licenseCheck != NULL)
    {
        ((LicenseCheckLinux*)m_licenseCheck)->GetMacOfInterfaceWithLicense(macLicense, ifnameLicense);
        while (next)
        {
            if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                    (next->ifa_flags & IFF_LOOPBACK) == 0)
            {
                INetPort* networkPort = new NetPortLinux(next->ifa_name);
                if(networkPort->GetNetworkCardType() != NETWORK_CARD_TYPE_UNKNOWN)
                {
                    networkPort->Initialize();
                    m_networkPorts.push_back(networkPort);

                    for (std::vector<TimeAwareSystem*>::size_type i = 0; i < m_timeAwareSystems.size(); ++i)
                    {
                        PortIdentity portIdentity;
                        PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), portIdentity.clockIdentity);
                        portIdentity.portNumber = m_networkPorts.size();
                        m_timeAwareSystems[i]->AddSystemPort(portIdentity);

                        lognotice("Sending on interface: %s", next->ifa_name);

                        if(strcmp(ifnameLicense, next->ifa_name) == 0)
                        {
                            uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                            PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC);
                            m_timeAwareSystems[i]->SetClockIdentity(clockIdentityFromMAC);
                            m_timeAwareSystems[i]->InitLocalClock(networkPort->GetPtpClock(), ((NetPortLinux*)networkPort)->GetPtpClockIndex());
                        }
                    }
                }
                else
                    delete networkPort;
            }
            next = next->ifa_next;
        }
    }
    freeifaddrs(addrs);
}

void TimeSyncDaemonLinux::DeleteNetworkPorts()
{
    for (std::vector<INetPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
        delete m_networkPorts[i];
    m_networkPorts.clear();
}

#endif //__linux__
