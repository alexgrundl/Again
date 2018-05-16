#include "timesyncdaemon.h"

TimeSyncDaemon::TimeSyncDaemon(int nDomains)
{
    m_licenseCheck = NULL;
    for (int i = 0; i < nDomains; ++i)
    {
        m_timeAwareSystems.push_back(new TimeAwareSystem());
        m_timeAwareSystems[i]->SetDomain(i);
    }
}

TimeSyncDaemon::~TimeSyncDaemon()
{
    DeleteManagers();

    for (std::vector<TimeAwareSystem*>::size_type i = 0; i < m_timeAwareSystems.size(); ++i)
        delete m_timeAwareSystems[i];
    m_timeAwareSystems.clear();
}

void TimeSyncDaemon::InitializeManagers()
{
    DeleteManagers();

    for (std::vector<StateMachineManager*>::size_type i = 0; i < m_timeAwareSystems.size(); ++i)
    {
        m_stateManagers.push_back(new StateMachineManager());
        m_stateManagers[i]->Initialize(m_timeAwareSystems[i], m_networkPorts);
    }

    for (std::vector<INetPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
        m_portManagers.push_back(new PortManager(m_networkPorts[i], m_stateManagers, i));
}

void TimeSyncDaemon::DeleteManagers()
{
    for (std::vector<StateMachineManager*>::size_type i = 0; i < m_stateManagers.size(); ++i)
        delete m_stateManagers[i];
    m_stateManagers.clear();

    for (std::vector<INetPort*>::size_type i = 0; i < m_portManagers.size(); ++i)
        delete m_portManagers[i];
    m_portManagers.clear();
}

bool TimeSyncDaemon::LicenseValid()
{
    bool checkLicense = true;
#if defined(NO_LICENSE_CHECK) || defined(__arm__)
    checkLicense = false;
#endif

    return !checkLicense || m_licenseCheck->LicenseValid();
}

void TimeSyncDaemon::Start()
{
    for (std::vector<StateMachineManager*>::size_type i = 0; i < m_stateManagers.size(); ++i)
       m_stateManagers[i]->StartProcessing();
    for (std::vector<INetPort*>::size_type i = 0; i < m_networkPorts.size(); ++i)
        m_portManagers[i]->StartReceiving();
}
