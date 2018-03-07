#include "statemachinemanager.h"

StateMachineManager::StateMachineManager(TimeAwareSystem* timeAwareSystem, std::vector<std::shared_ptr<PortGlobal>> ports,
                                         std::vector<std::shared_ptr<INetworkInterfacePort>> networkPorts)
{
    m_timeAwareSystem = timeAwareSystem;
    m_currentIndexClockUpdate = 0;

    m_timeAwareSystem->localClockTickInterval.ns = (uint64_t)m_granularity_ms * 1000 * 1000;
    m_timeAwareSystem->localClockTickInterval.ns_frac = 0;

    for (std::vector<std::shared_ptr<PortGlobal>>::size_type i = 0; i < ports.size(); ++i)
    {
        m_mdPdelayReq.push_back(std::shared_ptr<MDPdelayReq>(new MDPdelayReq(timeAwareSystem, ports[i].get(), networkPorts[i].get())));
        m_mdPdelayResp.push_back(std::shared_ptr<MDPdelayResp>(new MDPdelayResp(timeAwareSystem, ports[i].get(), networkPorts[i].get())));

        m_mdSyncSendSM.push_back(std::shared_ptr<MDSyncSendSM>(new MDSyncSendSM(timeAwareSystem, ports[i].get(), networkPorts[i].get())));
        m_portSyncSyncSends.push_back(std::shared_ptr<PortSyncSyncSend>(new PortSyncSyncSend(timeAwareSystem, ports[i].get(), m_mdSyncSendSM[i])));
    }

    m_clockSlaveSync = std::shared_ptr<ClockSlaveSync>(new ClockSlaveSync(timeAwareSystem));
    m_siteSyncSync = std::shared_ptr<SiteSyncSync>(new SiteSyncSync(timeAwareSystem, m_clockSlaveSync, m_portSyncSyncSends));
    m_clockMasterSyncSend = std::shared_ptr<ClockMasterSyncSend>(new ClockMasterSyncSend(timeAwareSystem, m_siteSyncSync));
    m_clockMasterSyncReceive = std::shared_ptr<ClockMasterSyncReceive>(new ClockMasterSyncReceive(m_timeAwareSystem));
    m_clockMasterSyncOffset = std::shared_ptr<ClockMasterSyncOffset>(new ClockMasterSyncOffset(m_timeAwareSystem));

    for (std::vector<std::shared_ptr<PortGlobal>>::size_type i = 0; i < ports.size(); ++i)
    {
        m_portSyncSyncReceive.push_back(std::shared_ptr<PortSyncSyncReceive>(new PortSyncSyncReceive(timeAwareSystem, ports[i].get(), m_siteSyncSync)));
        m_mdSyncReceiveSM.push_back(std::shared_ptr<MDSyncReceiveSM>(new MDSyncReceiveSM(timeAwareSystem, ports[i].get(),
                                                m_portSyncSyncReceive[i], networkPorts[i].get())));
    }

    for (std::vector<std::shared_ptr<PortGlobal>>::size_type i = 0; i < ports.size(); ++i)
    {
        m_portAnnounceInformation.push_back(std::shared_ptr<PortAnnounceInformation>(new PortAnnounceInformation(timeAwareSystem, ports[i].get())));
        m_portAnnounceReceive.push_back(std::shared_ptr<PortAnnounceReceive>(new PortAnnounceReceive(
                                                                                 timeAwareSystem, ports[i].get(), m_portAnnounceInformation[i])));
        m_mdPortAnnounceTransmit.push_back(std::shared_ptr<MDPortAnnounceTransmit>(new MDPortAnnounceTransmit(m_timeAwareSystem, ports[i].get(), networkPorts[i].get())));
        m_portAnnounceTransmit.push_back(std::shared_ptr<PortAnnounceTransmit>(new PortAnnounceTransmit(m_timeAwareSystem, ports[i].get(), m_mdPortAnnounceTransmit[i])));
    }
    m_portRoleSelection = std::shared_ptr<PortRoleSelection>(new PortRoleSelection(m_timeAwareSystem, ports));

    m_stateMachines.push_back(m_siteSyncSync);
    m_stateMachines.push_back(m_clockSlaveSync);
    m_stateMachines.push_back(m_clockMasterSyncSend);
    m_stateMachines.push_back(m_clockMasterSyncReceive);
    m_stateMachines.push_back(m_clockMasterSyncOffset);


    for (std::vector<std::shared_ptr<PortGlobal>>::size_type i = 0; i < ports.size(); ++i)
    {
        m_stateMachines.push_back(m_mdPdelayReq[i]);
        m_stateMachines.push_back(m_mdPdelayResp[i]);
        m_stateMachines.push_back(m_mdSyncReceiveSM[i]);
        m_stateMachines.push_back(m_portSyncSyncReceive[i]);
        m_stateMachines.push_back(m_portSyncSyncSends[i]);
        m_stateMachines.push_back(m_mdSyncSendSM[i]);

        m_stateMachines.push_back(m_portAnnounceInformation[i]);
        m_stateMachines.push_back(m_portAnnounceReceive[i]);
        m_stateMachines.push_back(m_mdPortAnnounceTransmit[i]);
        m_stateMachines.push_back(m_portAnnounceTransmit[i]);
    }
    m_stateMachines.push_back(m_portRoleSelection);

    m_stateThread = new CThreadWrapper<StateMachineManager>(this, &StateMachineManager::Process, std::string("State machine thread"));
}

StateMachineManager::~StateMachineManager()
{
    m_stateThread->Stop();
    delete m_stateThread;
    m_stateThread = NULL;
}

void StateMachineManager::StartProcessing()
{
    m_stateThread->Start();
}

uint32_t StateMachineManager::Process(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    InitialProcess();
    while(*pbIsRunning)
    {
      dwWaitResult = pal::EventWait(pWaitHandle, m_timeAwareSystem->localClockTickInterval.ns / 1000000);
      if(dwWaitResult == pal::EventWaitTimeout)
      {
          UpdateTimes();

          for (std::vector<StateMachineBase>::size_type i = 0; i < m_stateMachines.size(); ++i)
          {
              m_stateMachines[i]->ProcessState();
          }
      }
    }

    return 0;
}


void StateMachineManager::InitialProcess()
{
    for (std::vector<StateMachineBase>::size_type i = 0; i < m_stateMachines.size(); ++i)
    {
        m_stateMachines[i]->ProcessState();
    }
    if(m_stateMachines.size() > 0)
        m_stateMachines[0]->GetTimeAwareSystem()->BEGIN = false;
}


void StateMachineManager::ProcessPackage(int portIndex, IReceivePackage* package)
{
    PtpMessageType messageType = PtpMessageBase::ParseMessageType(package->GetBuffer());
    int domain = PtpMessageBase::ParseDomain(package->GetBuffer());

    /* Remove in good code. */
//    ((CLinuxReceivePackage*)package)->SetTimestamp(m_timeAwareSystem->GetCurrentTime());

    if(domain == 0)
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
        }
    }
}

void StateMachineManager::UpdateTimes()
{

    PtpClock clock;

    m_currentIndexClockUpdate++;
    if(m_currentIndexClockUpdate == m_clockSourceTimeUpdate)
    {
        m_currentIndexClockUpdate = 0;

        clock.Invoke(&clockSourceParams);
        m_clockMasterSyncReceive->SetClockSourceRequest(&clockSourceParams);
    }
    else
        m_clockMasterSyncReceive->SignalLocalClockUpdate();
}
