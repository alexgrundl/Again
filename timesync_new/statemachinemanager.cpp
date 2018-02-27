#include "statemachinemanager.h"

StateMachineManager::StateMachineManager(TimeAwareSystem* timeAwareSystem, PortGlobal* port,
                                         MDGlobal* mdGlobal, INetworkInterfacePort* networkPort)
{
    m_timeAwareSystem = timeAwareSystem;

    m_mdPdelayReq = new MDPdelayReq(timeAwareSystem, port, mdGlobal, networkPort);
    m_mdSyncSendSM = new MDSyncSendSM(timeAwareSystem, port, mdGlobal, networkPort);
    m_portSyncSyncSend = new PortSyncSyncSend(timeAwareSystem, port, m_mdSyncSendSM);
    m_portSyncSyncSends.push_back(m_portSyncSyncSend);

    m_clockSlaveSync = new ClockSlaveSync(timeAwareSystem);
    m_siteSyncSync = new SiteSyncSync(timeAwareSystem, m_clockSlaveSync, m_portSyncSyncSends);
    m_portSyncSyncReceive = new PortSyncSyncReceive(timeAwareSystem, port, m_siteSyncSync);
    m_clockMasterSyncSend = new ClockMasterSyncSend(timeAwareSystem, m_siteSyncSync);
    m_mdSyncReceiveSM = new MDSyncReceiveSM(timeAwareSystem, port, mdGlobal,
                                            m_portSyncSyncReceive, networkPort);


    m_stateMachines.push_back(m_mdSyncReceiveSM);
    m_stateMachines.push_back(m_portSyncSyncReceive);
    m_stateMachines.push_back(m_siteSyncSync);
    m_stateMachines.push_back(m_clockSlaveSync);
    m_stateMachines.push_back(m_clockMasterSyncSend);
    m_stateMachines.push_back(m_portSyncSyncSend);
    m_stateMachines.push_back(m_mdSyncSendSM);
    m_stateMachines.push_back(m_mdPdelayReq);

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
      dwWaitResult = pal::EventWait(pWaitHandle, m_granularity_ms);
      if(dwWaitResult == pal::EventWaitTimeout)
      {
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


void StateMachineManager::ProcessPackage(IReceivePackage* package)
{
    PtpMessageType messageType = PtpMessageBase::ParseMessageType(package->GetBuffer());

    /* Remove in good code. */
    ((CLinuxReceivePackage*)package)->SetTimestamp(m_timeAwareSystem->GetCurrentTime());

    switch(messageType)
    {
    case PTP_MESSSAGE_TYPE_PDELAY_RESP:
        m_mdPdelayReq->SetPDelayResponse(package);
        break;
    case PTP_MESSSAGE_TYPE_PDELAY_RESP_FOLLOW_UP:
        m_mdPdelayReq->SetPDelayResponseFollowUp(package);
        break;
    }
}
