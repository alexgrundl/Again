#include "sitesyncsync.h"

SiteSyncSync::SiteSyncSync(TimeAwareSystem* timeAwareSystem, ClockSlaveSync* clockSlaveSync, std::vector<PortSyncSyncSend*> portSyncSyncSends) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdPSSyncPtr = new PortSyncSync();

    m_clockSlaveSync = clockSlaveSync;
    for (std::vector<std::shared_ptr<PortSyncSyncSend>>::size_type i = 0; i < portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends.push_back(portSyncSyncSends[i]);
    }
}

SiteSyncSync::~SiteSyncSync()
{
    delete m_rcvdPSSyncPtr;
    for (std::deque<PortSyncSync*>::size_type i = 0; i < m_txPSSyncPtrs.size(); ++i)
    {
        PortSyncSync* portSyncSync;
        m_txPSSyncPtrs.pop_front(portSyncSync);
        delete portSyncSync;
    }
}

void SiteSyncSync::SetPSSyncSend (PortSyncSync* rcvdPSSyncIndPtr)
{
    PortSyncSync* txPSSyncPtr = new PortSyncSync();
    txPSSyncPtr->followUpCorrectionField = rcvdPSSyncIndPtr->followUpCorrectionField;
    txPSSyncPtr->gmTimeBaseIndicator = rcvdPSSyncIndPtr->gmTimeBaseIndicator;
    txPSSyncPtr->lastGmFreqChange = rcvdPSSyncIndPtr->lastGmFreqChange;
    txPSSyncPtr->lastGmPhaseChange = rcvdPSSyncIndPtr->lastGmPhaseChange;
    txPSSyncPtr->localPortNumber = rcvdPSSyncIndPtr->localPortNumber;
    txPSSyncPtr->logMessageInterval = rcvdPSSyncIndPtr->logMessageInterval;
    txPSSyncPtr->preciseOriginTimestamp = rcvdPSSyncIndPtr->preciseOriginTimestamp;
    txPSSyncPtr->rateRatio = rcvdPSSyncIndPtr->rateRatio;
    txPSSyncPtr->sourcePortIdentity = rcvdPSSyncIndPtr->sourcePortIdentity;
    txPSSyncPtr->syncReceiptTimeoutTime = rcvdPSSyncIndPtr->syncReceiptTimeoutTime;
    txPSSyncPtr->upstreamTxTime = rcvdPSSyncIndPtr->upstreamTxTime;

    m_txPSSyncPtrs.push_back(txPSSyncPtr);
}

void SiteSyncSync::TxPSSync()
{
    for (std::deque<PortSyncSync*>::size_type i = 0; i < m_txPSSyncPtrs.size(); ++i)
    {
        PortSyncSync* txPSSyncPtr;
        m_txPSSyncPtrs.pop_front(txPSSyncPtr);
        m_clockSlaveSync->SetPortSyncSync(txPSSyncPtr);
        for (std::vector<PortSyncSyncSend*>::size_type j = 0; j < m_portSyncSyncSends.size(); ++j)
        {
            m_portSyncSyncSends[j]->ProcessSync(txPSSyncPtr);
        }
    }
}

void SiteSyncSync::SetSync(PortSyncSync* rcvd)
{
    m_rcvdPSSyncPtr->followUpCorrectionField = rcvd->followUpCorrectionField;
    m_rcvdPSSyncPtr->gmTimeBaseIndicator = rcvd->gmTimeBaseIndicator;
    m_rcvdPSSyncPtr->lastGmFreqChange = rcvd->lastGmFreqChange;
    m_rcvdPSSyncPtr->lastGmPhaseChange = rcvd->lastGmPhaseChange;
    m_rcvdPSSyncPtr->localPortNumber = rcvd->localPortNumber;
    m_rcvdPSSyncPtr->logMessageInterval = rcvd->logMessageInterval;
    m_rcvdPSSyncPtr->preciseOriginTimestamp = rcvd->preciseOriginTimestamp;
    m_rcvdPSSyncPtr->rateRatio = rcvd->rateRatio;
    m_rcvdPSSyncPtr->sourcePortIdentity = rcvd->sourcePortIdentity;
    m_rcvdPSSyncPtr->syncReceiptTimeoutTime = rcvd->syncReceiptTimeoutTime;
    m_rcvdPSSyncPtr->upstreamTxTime = rcvd->upstreamTxTime;
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
        if(m_rcvdPSSync && m_timeAwareSystem->GetSelectedRole(m_rcvdPSSyncPtr->localPortNumber) == PORT_ROLE_SLAVE
                &&  m_timeAwareSystem->IsGmPresent())
        {
            m_rcvdPSSync = false;
            SetPSSyncSend(m_rcvdPSSyncPtr);
            TxPSSync();

            m_state = STATE_RECEIVING_SYNC;
        }
    }
    pal::LockedRegionLeave(m_lock);
}
