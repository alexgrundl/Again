#include "clockmastersyncsend.h"

ClockMasterSyncSend::ClockMasterSyncSend(TimeAwareSystem* timeAwareSystem, SiteSyncSync* siteSyncSync) :
    StateMachineBase(timeAwareSystem)
{
    m_syncSendTime.ns = 0;
    m_syncSendTime.ns_frac = 0;
    m_txPSSyncPtr = new PortSyncSync();

    m_siteSyncSync = siteSyncSync;
}

ClockMasterSyncSend::~ClockMasterSyncSend()
{
    delete m_txPSSyncPtr;
}

void ClockMasterSyncSend::SetPSSyncCMSS(double gmRateRatio)
{
    ExtendedTimestamp masterTime = m_timeAwareSystem->GetMasterTime();
    UScaledNs localTime = m_timeAwareSystem->GetLocalTime();

    m_txPSSyncPtr->localPortNumber = 0;
    m_txPSSyncPtr->preciseOriginTimestamp.sec = masterTime.sec;
    m_txPSSyncPtr->preciseOriginTimestamp.ns = masterTime.ns;

    /* We remove this for now as this seems wrong. "masterTime" and "localTime" have the resolution given through the calls
     * to "ClockMasterSyncReceive.SetClockSourceRequest()". Current time is free running. So, when sending the follow up message,
     * we calculate the correction field wrongly, then!
     */
//    m_txPSSyncPtr->followUpCorrectionField = (ScaledNs)(m_timeAwareSystem->GetCurrentTime() - localTime) * m_timeAwareSystem->GetGmRateRatio();
//    m_txPSSyncPtr->followUpCorrectionField.ns_frac += masterTime.ns_frac;

    m_txPSSyncPtr->followUpCorrectionField = {0, 0};
    memcpy(m_txPSSyncPtr->sourcePortIdentity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH);
    m_txPSSyncPtr->sourcePortIdentity.portNumber = 0;
    m_txPSSyncPtr->logMessageInterval = m_timeAwareSystem->GetClockMasterLogSyncInterval();
    m_txPSSyncPtr->upstreamTxTime = localTime;
    m_txPSSyncPtr->syncReceiptTimeoutTime.ns = 0xFFFFFFFFFFFF;
    m_txPSSyncPtr->syncReceiptTimeoutTime.ns_frac = 0xFFFF;
    m_txPSSyncPtr->rateRatio = gmRateRatio;
    m_txPSSyncPtr->gmTimeBaseIndicator = m_timeAwareSystem->GetClockSourceTimeBaseIndicator();
    m_txPSSyncPtr->lastGmPhaseChange = m_timeAwareSystem->GetClockSourcePhaseOffset();
    m_txPSSyncPtr->lastGmFreqChange = m_timeAwareSystem->GetClockSourceFreqOffset();
    m_txPSSyncPtr->domain = m_timeAwareSystem->GetDomain();
}

void ClockMasterSyncSend::TxPSSyncCMSS()
{
    m_siteSyncSync->SetSync(m_txPSSyncPtr);
}

void ClockMasterSyncSend::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_syncSendTime = m_timeAwareSystem->GetCurrentTime();
        m_syncSendTime.ns += NS_PER_SEC * pow(2, m_timeAwareSystem->GetClockMasterLogSyncInterval());
        m_state = STATE_INITIALIZING;
    }
    else
    {
        if(m_timeAwareSystem->GetCurrentTime() >= m_syncSendTime)
        {
            m_state = STATE_SEND_SYNC_INDICATION;

            SetPSSyncCMSS(m_timeAwareSystem->GetGmRateRatio());
            TxPSSyncCMSS();
            m_syncSendTime = m_timeAwareSystem->GetCurrentTime();
            m_syncSendTime.ns += NS_PER_SEC * pow(2, m_timeAwareSystem->GetClockMasterLogSyncInterval());
        }
    }
}

