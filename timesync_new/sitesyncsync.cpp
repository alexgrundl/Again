#include "sitesyncsync.h"

SiteSyncSync::SiteSyncSync(TimeAwareSystem* timeAwareSystem, std::shared_ptr<ClockSlaveSync> clockSlaveSync, std::vector<std::shared_ptr<PortSyncSyncSend>> portSyncSyncSends) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdPSSyncPtr = NULL;
    m_txPSSyncPtr = std::unique_ptr<PortSyncSync>(new PortSyncSync());

    m_clockSlaveSync = clockSlaveSync;
    for (std::vector<std::shared_ptr<PortSyncSyncSend>>::size_type i = 0; i < portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends.push_back(portSyncSyncSends[i]);
    }
}

SiteSyncSync::~SiteSyncSync()
{
}

void SiteSyncSync::SetPSSyncSend (PortSyncSync* rcvdPSSyncIndPtr)
{
     m_txPSSyncPtr->followUpCorrectionField = rcvdPSSyncIndPtr->followUpCorrectionField;
     m_txPSSyncPtr->gmTimeBaseIndicator = rcvdPSSyncIndPtr->gmTimeBaseIndicator;
     m_txPSSyncPtr->lastGmFreqChange = rcvdPSSyncIndPtr->lastGmFreqChange;
     m_txPSSyncPtr->lastGmPhaseChange = rcvdPSSyncIndPtr->lastGmPhaseChange;
     m_txPSSyncPtr->localPortNumber = rcvdPSSyncIndPtr->localPortNumber;
     m_txPSSyncPtr->logMessageInterval = rcvdPSSyncIndPtr->logMessageInterval;
     m_txPSSyncPtr->preciseOriginTimestamp = rcvdPSSyncIndPtr->preciseOriginTimestamp;
     m_txPSSyncPtr->rateRatio = rcvdPSSyncIndPtr->rateRatio;
     m_txPSSyncPtr->sourcePortIdentity = rcvdPSSyncIndPtr->sourcePortIdentity;
     m_txPSSyncPtr->syncReceiptTimeoutTime = rcvdPSSyncIndPtr->syncReceiptTimeoutTime;
     m_txPSSyncPtr->upstreamTxTime = rcvdPSSyncIndPtr->upstreamTxTime;
}

void SiteSyncSync::TxPSSync()
{
    m_clockSlaveSync->SetPortSyncSync(m_txPSSyncPtr.get());
    for (std::vector<PortSyncSyncSend*>::size_type i = 0; i < m_portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends[i]->ProcessSync(m_txPSSyncPtr.get());
    }

}

void SiteSyncSync::SetSync(PortSyncSync* rcvd)
{
    m_rcvdPSSyncPtr = rcvd;
    m_rcvdPSSync = true;
}

void SiteSyncSync::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_rcvdPSSync = false;
    }
    else
    {
        if(m_rcvdPSSync && m_timeAwareSystem->selectedRole[m_rcvdPSSyncPtr->localPortNumber] == PORT_ROLE_SLAVE
                &&  m_timeAwareSystem->gmPresent)
        {
            m_rcvdPSSync = false;
            SetPSSyncSend(m_rcvdPSSyncPtr);
            TxPSSync();

            m_state = STATE_RECEIVING_SYNC;
        }
    }
}
