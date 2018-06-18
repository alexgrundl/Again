#include "statemachinemanager.h"

StateMachineManager::StateMachineManager()
{
}

StateMachineManager::~StateMachineManager()
{
    m_stateThread->Stop();
    delete m_stateThread;
    m_stateThread = NULL;

    for (std::vector<MDPdelayReq*>::size_type i = 0; i < m_mdPdelayReq.size(); ++i)
    {
        delete m_mdPdelayReq[i];
        delete m_mdPdelayResp[i];
        delete m_mdSyncSendSM[i];
        delete m_portSyncSyncSends[i];
        delete m_portSyncSyncReceive[i];
        delete m_mdSyncReceiveSM[i];
        delete m_portAnnounceInformation[i];
        delete m_portAnnounceReceive[i];
        delete m_mdPortAnnounceTransmit[i];
        delete m_portAnnounceTransmit[i];
#ifdef __linux__
        delete m_portIPC[i];
#endif
    }

    delete m_clockSlaveSync;
    delete m_siteSyncSync;
    delete m_clockMasterSyncSend;
    delete m_clockMasterSyncReceive;
    delete m_clockMasterSyncOffset;
    delete m_portRoleSelection;

    for (std::vector<PlatformSync*>::size_type i = 0; i < m_platformSync.size(); ++i)
    {
        delete m_platformSync[i];
    }
}

void StateMachineManager::Initialize(TimeAwareSystem* timeAwareSystem, std::vector<INetPort*> networkPorts)
{
    m_timeAwareSystem = timeAwareSystem;
    m_currentIndexClockUpdate = 0;

    m_timeAwareSystem->SetLocalClockTickInterval({(uint64_t)m_granularity_ms * 1000 * 1000, 0});

    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_mdPdelayReq.push_back(new MDPdelayReq(timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));
        m_mdPdelayResp.push_back(new MDPdelayResp(timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));

        m_mdSyncSendSM.push_back(new MDSyncSendSM(timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));
        m_portSyncSyncSends.push_back(new PortSyncSyncSend(timeAwareSystem, timeAwareSystem->GetSystemPort(i), m_mdSyncSendSM[i]));
    }

    m_clockMasterSyncReceive = new ClockMasterSyncReceive(m_timeAwareSystem);
    m_clockMasterSyncOffset = new ClockMasterSyncOffset(m_timeAwareSystem);
    m_clockSlaveSync = new ClockSlaveSync(timeAwareSystem, m_clockMasterSyncOffset);
    m_siteSyncSync = new SiteSyncSync(timeAwareSystem, m_clockSlaveSync, m_portSyncSyncSends);
    m_clockMasterSyncSend = new ClockMasterSyncSend(timeAwareSystem, m_siteSyncSync);

    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_portSyncSyncReceive.push_back(new PortSyncSyncReceive(timeAwareSystem, timeAwareSystem->GetSystemPort(i), m_siteSyncSync));
        m_mdSyncReceiveSM.push_back(new MDSyncReceiveSM(timeAwareSystem, timeAwareSystem->GetSystemPort(i), m_portSyncSyncReceive[i], networkPorts[i]));
    }

    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_portAnnounceInformation.push_back(new PortAnnounceInformation(timeAwareSystem, timeAwareSystem->GetSystemPort(i)));
        m_portAnnounceReceive.push_back(new PortAnnounceReceive(timeAwareSystem, timeAwareSystem->GetSystemPort(i), m_portAnnounceInformation[i]));
        m_mdPortAnnounceTransmit.push_back(new MDPortAnnounceTransmit(m_timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));
        m_portAnnounceTransmit.push_back(new PortAnnounceTransmit(m_timeAwareSystem, timeAwareSystem->GetSystemPort(i), m_mdPortAnnounceTransmit[i]));

        m_linkDelaySyncIntervalSetting.push_back(new LinkDelaySyncIntervalSetting(timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));
        if(m_timeAwareSystem->IsPTSSEnabled() && m_timeAwareSystem->GetDomain() == 0 &&
                (networkPorts[i]->GetNetworkCardType() == NETWORK_CARD_TYPE_X540 || networkPorts[i]->GetNetworkCardType() == NETWORK_CARD_TYPE_I210))
            m_platformSync.push_back(new PlatformSync(m_timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i]));
#ifdef __linux__
        m_portIPC.push_back(new PortIPC(m_timeAwareSystem, timeAwareSystem->GetSystemPort(i), networkPorts[i], m_timeAwareSystem->GetDomain()));
#endif
    }
    m_portRoleSelection = new PortRoleSelection(m_timeAwareSystem, networkPorts);

    m_stateMachines.push_back(m_siteSyncSync);
    m_stateMachines.push_back(m_clockSlaveSync);
    m_stateMachines.push_back(m_clockMasterSyncSend);
    m_stateMachines.push_back(m_clockMasterSyncReceive);
    m_stateMachines.push_back(m_clockMasterSyncOffset);

    if(m_timeAwareSystem->GetDomain() == 0)
    {
        for (std::vector<PlatformSync*>::size_type i = 0; i < m_platformSync.size(); ++i)
        {
            m_stateMachines.push_back(m_platformSync[i]);
        }
    }

    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_stateMachines.push_back(m_mdPdelayReq[i]);
        m_stateMachines.push_back(m_mdPdelayResp[i]);
        m_stateMachines.push_back(m_mdSyncReceiveSM[i]);
        m_stateMachines.push_back(m_portSyncSyncReceive[i]);
        m_stateMachines.push_back(m_portSyncSyncSends[i]);
        m_stateMachines.push_back(m_mdSyncSendSM[i]);

        m_stateMachines.push_back(m_portAnnounceReceive[i]);
        m_stateMachines.push_back(m_portAnnounceInformation[i]);

        m_stateMachines.push_back(m_linkDelaySyncIntervalSetting[i]);
#ifdef __linux__
        m_stateMachines.push_back(m_portIPC[i]);
#endif
    }
    m_stateMachines.push_back(m_portRoleSelection);

    for (std::vector<INetPort*>::size_type i = 0; i < networkPorts.size(); ++i)
    {
        m_stateMachines.push_back(m_mdPortAnnounceTransmit[i]);
        m_stateMachines.push_back(m_portAnnounceTransmit[i]);
    }

    m_stateThread = new CThreadWrapper<StateMachineManager>(this, &StateMachineManager::Process, std::string("State machine thread"));
}

void StateMachineManager::StartProcessing()
{
    m_stateThread->Start();
}

void StateMachineManager::StopProcessing()
{
    m_stateThread->Stop();
}

uint32_t StateMachineManager::Process(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    InitialProcess();
    while(*pbIsRunning)
    {
      dwWaitResult = pal::EventWait(pWaitHandle, m_timeAwareSystem->GetLocalClockTickInterval().ns / 1000000);
      if(dwWaitResult == pal::EventWaitTimeout)
      {
          for (std::vector<StateMachineBase*>::size_type i = 0; i < m_stateMachines.size(); ++i)
          {
              m_stateMachines[i]->ProcessState();
          }
      }
    }

    return 0;
}


void StateMachineManager::InitialProcess()
{
    for (std::vector<StateMachineBase*>::size_type i = 0; i < m_stateMachines.size(); ++i)
    {
        m_stateMachines[i]->ProcessState();
    }
    if(m_stateMachines.size() > 0)
        m_stateMachines[0]->GetTimeAwareSystem()->BEGIN = false;
}


void StateMachineManager::ProcessPackage(int portIndex, ReceivePackage *package)
{
    PtpMessageType messageType = PtpMessageBase::ParseMessageType(package->GetBuffer());
    int domain = PtpMessageBase::ParseDomain(package->GetBuffer());

    if(domain == m_timeAwareSystem->GetDomain())
    {
        switch(messageType)
        {
        case PTP_MESSSAGE_TYPE_PDELAY_REQ:
            m_mdPdelayResp[portIndex]->SetPDelayRequest(package);
            break;
        case PTP_MESSSAGE_TYPE_PDELAY_RESP:
            m_mdPdelayReq[portIndex]->SetPDelayResponse(package);
            break;
        case PTP_MESSSAGE_TYPE_PDELAY_RESP_FOLLOW_UP:
            m_mdPdelayReq[portIndex]->SetPDelayResponseFollowUp(package);
            break;
        case PTP_MESSSAGE_TYPE_SYNC:
            m_mdSyncReceiveSM[portIndex]->SetSyncMessage(package);
            break;
        case PTP_MESSSAGE_TYPE_FOLLOW_UP:
            m_mdSyncReceiveSM[portIndex]->SetFollowUpMessage(package);
            break;
        case PTP_MESSSAGE_TYPE_ANNOUNCE:
            m_portAnnounceReceive[portIndex]->SetAnnounce(package);
            break;
        case PTP_MESSSAGE_TYPE_SIGNALING:
            m_linkDelaySyncIntervalSetting[portIndex]->SetSignalingMessage(package);
            break;
        }
    }
}
