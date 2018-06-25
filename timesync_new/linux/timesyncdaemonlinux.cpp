#ifdef __linux__

#include "timesyncdaemonlinux.h"

TimeSyncDaemonLinux::TimeSyncDaemonLinux(PtpConfig *config) : TimeSyncDaemon(config)
{
    m_licenseCheck = new LicenseCheckLinux();
    if(config->GetClockTimeSource() == CLOCK_TIME_SOURCE_GPS)
    {
        m_serial.Open("/dev/ttyACM0");
        m_serial.SetAttributes(B115200, 0, false);
        m_gpsSync = new GPSSync(&m_serial, m_timeAwareSystems[0]);
    }
}

TimeSyncDaemonLinux::~TimeSyncDaemonLinux()
{
    delete m_licenseCheck;
    DeleteManagers();
    DeleteNetworkPorts();
    if(m_gpsSync != NULL)
    {
        m_gpsSync->StopSync();
        m_serial.Close();
        delete m_gpsSync;
    }
}

void TimeSyncDaemonLinux::InitalizePorts(PtpConfig* config)
{
    uint8_t macLicense[ETH_ALEN];
    char ifnameLicense[IFNAMSIZ];
    struct ifaddrs *addrs,*next;

    DeleteNetworkPorts();

    getifaddrs(&addrs);
    next = addrs;

    if(m_licenseCheck != NULL)
    {
        ((LicenseCheckLinux*)m_licenseCheck)->GetMacAndInterfaceWithLicense(macLicense, ifnameLicense);
        while (next)
        {
            if (next->ifa_addr && next->ifa_addr->sa_family == AF_PACKET &&
                    (next->ifa_flags & IFF_LOOPBACK) == 0)
            {
                INetPort* networkPort = new NetPortLinux(next->ifa_name);
                if(networkPort->GetNetworkCardType() != NETWORK_CARD_TYPE_UNKNOWN)
                {
                    networkPort->Initialize();
                    if(config->GetRxDelay(networkPort->GetMaxSpeed()) >= 0)
                        networkPort->SetRxPhyDelay(config->GetRxDelay(networkPort->GetMaxSpeed()));
                    if(config->GetTxDelay(networkPort->GetMaxSpeed()) >= 0)
                        networkPort->SetTxPhyDelay(config->GetTxDelay(networkPort->GetMaxSpeed()));

                    m_networkPorts.push_back(networkPort);

                    for (std::vector<TimeAwareSystem*>::size_type i = 0; i < m_timeAwareSystems.size(); ++i)
                    {
                        PortIdentity portIdentity;
                        PtpMessageBase::GetClockIdentity(macLicense, portIdentity.clockIdentity, m_timeAwareSystems[i]->GetDomain());

                        lognotice("Domain %u - Sending on interface: %s", m_timeAwareSystems[i]->GetDomain(), next->ifa_name);

#ifdef __arm__
                        memcpy(ifnameLicense, next->ifa_name, IFNAMSIZ);
#endif

                        portIdentity.portNumber = strcmp(ifnameLicense, next->ifa_name) == 0 ? 1 : m_networkPorts.size() + 1;
                        SystemPort* systemPort = m_timeAwareSystems[i]->AddSystemPort(portIdentity);
                        networkPort->AddSystemPort(systemPort);

                        if(strcmp(ifnameLicense, next->ifa_name) == 0)
                        {
                            lognotice("Domain %u - Main ethernet interface: %s", m_timeAwareSystems[i]->GetDomain(), next->ifa_name);
                            uint8_t clockIdentityFromMAC[CLOCK_ID_LENGTH];
                            PtpMessageBase::GetClockIdentity(networkPort->GetMAC(), clockIdentityFromMAC, m_timeAwareSystems[i]->GetDomain());
                            m_timeAwareSystems[i]->SetClockIdentity(clockIdentityFromMAC);
                            m_timeAwareSystems[i]->InitLocalClock(networkPort->GetPtpClock(), ((NetPortLinux*)networkPort)->GetPtpClockIndex());

                        }
                        else
                        {
                            /* If it's not the "main" port and "time relay" isn't enabled don't send any time sync frames at this port. */
                            if(!m_licenseCheck->IsTimeRelayEnabled())
                                networkPort->GetSystemPort(m_timeAwareSystems[i]->GetDomain())->DisablePttPort();
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

    /* After all ports have been initialized start GPS synchronization. */
    if(m_gpsSync != NULL)
        m_gpsSync->StartSync();
}

void TimeSyncDaemonLinux::DeleteNetworkPorts()
{
    for (std::vector<INetPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
        delete m_networkPorts[i];
    m_networkPorts.clear();
}

#endif //__linux__
