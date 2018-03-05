#include "clockmastersyncsend.h"

ClockMasterSyncSend::ClockMasterSyncSend(TimeAwareSystem* timeAwareSystem, std::shared_ptr<SiteSyncSync> siteSyncSync) :
    StateMachineBase(timeAwareSystem)
{
    m_syncSendTime.ns = 0;
    m_syncSendTime.ns_frac = 0;
    m_txPSSyncPtr = std::unique_ptr<PortSyncSync>(new PortSyncSync());

    m_siteSyncSync = siteSyncSync;
}

ClockMasterSyncSend::~ClockMasterSyncSend()
{
}

void ClockMasterSyncSend::SetPSSyncCMSS(double gmRateRatio)
{
    m_txPSSyncPtr->localPortNumber = 0;
    m_txPSSyncPtr->preciseOriginTimestamp.sec = m_timeAwareSystem->masterTime.sec;
    m_txPSSyncPtr->preciseOriginTimestamp.ns = m_timeAwareSystem->masterTime.ns;
    m_txPSSyncPtr->followUpCorrectionField = (ScaledNs)(m_timeAwareSystem->GetCurrentTime() - m_timeAwareSystem->localTime) * m_timeAwareSystem->gmRateRatio;
    m_txPSSyncPtr->followUpCorrectionField.ns_frac += m_timeAwareSystem->masterTime.ns_frac;
    memcpy(m_txPSSyncPtr->sourcePortIdentity.clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock));
    m_txPSSyncPtr->sourcePortIdentity.portNumber = 0;
    m_txPSSyncPtr->logMessageInterval = m_timeAwareSystem->clockMasterLogSyncInterval;
    m_txPSSyncPtr->upstreamTxTime = m_timeAwareSystem->localTime;
    m_txPSSyncPtr->syncReceiptTimeoutTime.ns = 0xFFFFFFFFFFFF;
    m_txPSSyncPtr->syncReceiptTimeoutTime.ns_frac = 0xFFFF;
    m_txPSSyncPtr->rateRatio = gmRateRatio;
    m_txPSSyncPtr->gmTimeBaseIndicator = m_timeAwareSystem->clockSourceTimeBaseIndicator;
    m_txPSSyncPtr->lastGmPhaseChange = m_timeAwareSystem->clockSourcePhaseOffset;
    m_txPSSyncPtr->lastGmFreqChange = m_timeAwareSystem->clockSourceFreqOffset;
}

void ClockMasterSyncSend::TxPSSyncCMSS()
{
    m_siteSyncSync->SetSync(m_txPSSyncPtr.get());
}

void ClockMasterSyncSend::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_syncSendTime = m_timeAwareSystem->GetCurrentTime();
        m_syncSendTime.ns += 1000000000 * pow(2, m_timeAwareSystem->clockMasterLogSyncInterval);
        m_state = STATE_INITIALIZING;
    }
    else
    {
        if(m_timeAwareSystem->GetCurrentTime() >= m_syncSendTime)
        {
            m_state = STATE_SEND_SYNC_INDICATION;

            SetPSSyncCMSS(m_timeAwareSystem->gmRateRatio);
            TxPSSyncCMSS();
            m_syncSendTime = m_timeAwareSystem->GetCurrentTime();
            m_syncSendTime.ns += 1000000000 * pow(2, m_timeAwareSystem->clockMasterLogSyncInterval);
        }
    }
}

