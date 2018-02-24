#include "clockmastersyncsend.h"

ClockMasterSyncSend::ClockMasterSyncSend(TimeAwareSystem* timeAwareSystem, SiteSyncSync* siteSyncSync) :
    StateMachineBase(timeAwareSystem)
{
    m_syncSendTime.ns = 0;
    m_syncSendTime.ns_frac = 0;
    m_txPSSyncPtr = NULL;

    m_siteSyncSync = siteSyncSync;
}

ClockMasterSyncSend::~ClockMasterSyncSend()
{
    delete m_txPSSyncPtr;
}

PortSyncSync* ClockMasterSyncSend::SetPSSyncCMSS(double gmRateRatio)
{
    PortSyncSync* txPSSyncPtr = new PortSyncSync();

    txPSSyncPtr->localPortNumber = 0;
    txPSSyncPtr->preciseOriginTimestamp.sec = m_timeAwareSystem->masterTime.sec;
    txPSSyncPtr->preciseOriginTimestamp.ns = m_timeAwareSystem->masterTime.ns;
    txPSSyncPtr->followUpCorrectionField = (ScaledNs)(m_timeAwareSystem->GetCurrentTime() - m_timeAwareSystem->localTime) * m_timeAwareSystem->gmRateRatio;
    txPSSyncPtr->followUpCorrectionField.ns_frac += m_timeAwareSystem->masterTime.ns_frac;
    memcpy(txPSSyncPtr->sourcePortIdentity.clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock));
    txPSSyncPtr->sourcePortIdentity.portNumber = 0;
    txPSSyncPtr->logMessageInterval = m_timeAwareSystem->clockMasterLogSyncInterval;
    txPSSyncPtr->upstreamTxTime = m_timeAwareSystem->localTime;
    txPSSyncPtr->syncReceiptTimeoutTime.ns = 0xFFFFFFFFFFFF;
    txPSSyncPtr->syncReceiptTimeoutTime.ns_frac = 0xFFFF;
    txPSSyncPtr->rateRatio = gmRateRatio;
    txPSSyncPtr->gmTimeBaseIndicator = m_timeAwareSystem->clockSourceTimeBaseIndicator;
    txPSSyncPtr->lastGmPhaseChange = m_timeAwareSystem->clockSourcePhaseOffset;
    txPSSyncPtr->lastGmFreqChange = m_timeAwareSystem->clockSourceFreqOffset;

    return txPSSyncPtr;
}

void ClockMasterSyncSend::TxPSSyncCMSS(PortSyncSync* txPSSyncPtr)
{
    m_siteSyncSync->ProcessStruct(txPSSyncPtr);
}

void ClockMasterSyncSend::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_syncSendTime = m_timeAwareSystem->GetCurrentTime() + m_timeAwareSystem->clockMasterSyncInterval;
        m_state = STATE_INITIALIZING;
    }
    else
    {
        if(m_timeAwareSystem->GetCurrentTime() >= m_syncSendTime)
        {
            m_state = STATE_SEND_SYNC_INDICATION;

            m_txPSSyncPtr = SetPSSyncCMSS (m_timeAwareSystem->gmRateRatio);
            TxPSSyncCMSS(m_txPSSyncPtr);
            m_syncSendTime = m_timeAwareSystem->GetCurrentTime() + m_timeAwareSystem->clockMasterSyncInterval;
        }
    }
}

