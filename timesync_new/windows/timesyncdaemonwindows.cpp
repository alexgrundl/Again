#include "timesyncdaemonwindows.h"

#ifndef __linux__

TimeSyncDaemonWindows::TimeSyncDaemonWindows(PtpConfig* config) : TimeSyncDaemon(config)
{
    m_licenseCheck = new LicenseCheckWindows();
}

TimeSyncDaemonWindows::~TimeSyncDaemonWindows()
{
    delete m_licenseCheck;
    DeleteManagers();
    DeleteNetworkPorts();
}

void TimeSyncDaemonWindows::InitalizePorts()
{
}

void TimeSyncDaemonWindows::DeleteNetworkPorts()
{
    for (std::vector<INetPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
        delete m_networkPorts[i];
    m_networkPorts.clear();
}

#endif //__linux__
