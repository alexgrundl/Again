#include "timesyncdaemon.h"

TimeSyncDaemon::TimeSyncDaemon(PtpConfig* config)
{
    m_licenseCheck = NULL;
    m_config = config;
    m_gpsSync = NULL;

    int nDomains = config->IsDomain1Enabled() ? 2 : 1;
    for (int i = 0; i < nDomains; ++i)
    {
        m_timeAwareSystems.push_back(new TimeAwareSystem());
        m_timeAwareSystems[i]->SetDomain(i);
        m_timeAwareSystems[i]->SetSystemPriority1(i == 0 ? config->GetPriority1Domain0() : config->GetPriority1Domain1());
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

    for (std::vector<TimeAwareSystem*>::size_type i = 0; i < m_timeAwareSystems.size(); ++i)
    {
        SetFeatures(m_timeAwareSystems[i]);
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

    if(m_config->GetLicenseMac().size() > 0)
        m_licenseCheck->SetMacWithLicense(m_config->GetLicenseMac());

    return !checkLicense || m_licenseCheck->LicenseValid();
}

void TimeSyncDaemon::Start()
{
    for (std::vector<StateMachineManager*>::size_type i = 0; i < m_stateManagers.size(); ++i)
       m_stateManagers[i]->StartProcessing();
    for (std::vector<PortManager*>::size_type i = 0; i < m_portManagers.size(); ++i)
        m_portManagers[i]->StartReceiving();
}

void TimeSyncDaemon::Stop()
{
    for (std::vector<StateMachineManager*>::size_type i = 0; i < m_stateManagers.size(); ++i)
        m_stateManagers[i]->StopProcessing();
    for (std::vector<PortManager*>::size_type i = 0; i < m_portManagers.size(); ++i)
        m_portManagers[i]->StopReceiving();
}

TimeAwareSystem* TimeSyncDaemon::GetTimeAwareSystem(int index)
{
    return index >= 0 && index < (int)m_timeAwareSystems.size() ? m_timeAwareSystems[index] : NULL;
}

void TimeSyncDaemon::SetFeatures(TimeAwareSystem* timeAwareSystem)
{
    timeAwareSystem->SetCTSSEnabled(m_licenseCheck->IsCTSSEnabled());
    timeAwareSystem->SetPTSSEnabled(m_licenseCheck->IsPTSSEnabled());
    timeAwareSystem->SetTimeRelayEnabled(m_licenseCheck->IsTimeRelayEnabled());
}
