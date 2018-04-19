#include "sitesyncsync.h"

SiteSyncSync::SiteSyncSync(TimeAwareSystem* timeAwareSystem, ClockSlaveSync* clockSlaveSync, std::vector<PortSyncSyncSend*> portSyncSyncSends) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_txPSSyncPtr = new PortSyncSync();

    m_clockSlaveSync = clockSlaveSync;
    for (std::vector<std::shared_ptr<PortSyncSyncSend>>::size_type i = 0; i < portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends.push_back(portSyncSyncSends[i]);
    }
}

SiteSyncSync::~SiteSyncSync()
{
    delete m_txPSSyncPtr;
    for (std::deque<PortSyncSync*>::size_type i = 0; i < m_rcvdPSSyncPtrs.size(); ++i)
    {
        PortSyncSync* portSyncSync;
        m_rcvdPSSyncPtrs.pop_front(portSyncSync);
        delete portSyncSync;
    }
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
    m_clockSlaveSync->SetPortSyncSync(m_txPSSyncPtr);
    for (std::vector<PortSyncSyncSend*>::size_type j = 0; j < m_portSyncSyncSends.size(); ++j)
    {
        m_portSyncSyncSends[j]->ProcessSync(m_txPSSyncPtr);
    }
}

void SiteSyncSync::SetSync(PortSyncSync* rcvd)
{
    PortSyncSync* rcvdPSSyncPtr = new PortSyncSync();

    rcvdPSSyncPtr->followUpCorrectionField = rcvd->followUpCorrectionField;
    rcvdPSSyncPtr->gmTimeBaseIndicator = rcvd->gmTimeBaseIndicator;
    rcvdPSSyncPtr->lastGmFreqChange = rcvd->lastGmFreqChange;
    rcvdPSSyncPtr->lastGmPhaseChange = rcvd->lastGmPhaseChange;
    rcvdPSSyncPtr->localPortNumber = rcvd->localPortNumber;
    rcvdPSSyncPtr->logMessageInterval = rcvd->logMessageInterval;
    rcvdPSSyncPtr->preciseOriginTimestamp = rcvd->preciseOriginTimestamp;
    rcvdPSSyncPtr->rateRatio = rcvd->rateRatio;
    rcvdPSSyncPtr->sourcePortIdentity = rcvd->sourcePortIdentity;
    rcvdPSSyncPtr->syncReceiptTimeoutTime = rcvd->syncReceiptTimeoutTime;
    rcvdPSSyncPtr->upstreamTxTime = rcvd->upstreamTxTime;
    m_rcvdPSSyncPtrs.push_back(rcvdPSSyncPtr);
    m_rcvdPSSync = true;
}

void SiteSyncSync::ProcessState()
{
    pal::LockedRegionEnter(m_lock);
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_rcvdPSSync = false;
    }
    else
    {
        if(m_rcvdPSSync &&  m_timeAwareSystem->IsGmPresent())
        {
            while(m_rcvdPSSyncPtrs.size() > 0)
            {
                PortSyncSync* rcvdPSSync;
                m_rcvdPSSyncPtrs.pop_front(rcvdPSSync);
                if(m_timeAwareSystem->GetSelectedRole(rcvdPSSync->localPortNumber) == PORT_ROLE_SLAVE)
                {
                    SetPSSyncSend(rcvdPSSync);
                    TxPSSync();

                    m_rcvdPSSync = false;
                    m_state = STATE_RECEIVING_SYNC;
                }
                delete rcvdPSSync;
            }
        }
    }
    pal::LockedRegionLeave(m_lock);
}
