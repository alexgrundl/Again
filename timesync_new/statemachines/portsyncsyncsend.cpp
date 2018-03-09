#include "portsyncsyncsend.h"

PortSyncSyncSend::PortSyncSyncSend(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDSyncSendSM *mdSyncSendSM) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_rcvdPSSync = false;
    m_rcvdPSSyncPtr = std::unique_ptr<PortSyncSync>(new PortSyncSync());
    memset(m_lastSourcePortIdentity.clockIdentity, 0, sizeof(m_lastSourcePortIdentity.clockIdentity));
    m_lastSourcePortIdentity.portNumber = 0;
    m_lastPreciseOriginTimestamp.sec = 0;
    m_lastPreciseOriginTimestamp.ns = 0;
    m_lastFollowUpCorrectionField.ns = 0;
    m_lastFollowUpCorrectionField.ns_frac = 0;
    m_lastRateRatio = 0.0;
    m_lastUpstreamTxTime.ns = 0;
    m_lastUpstreamTxTime.ns_frac = 0;
    m_lastSyncSentTime.ns = 0;
    m_lastSyncSentTime.ns_frac = 0;
    m_lastRcvdPortNum = 0;
    m_lastGmTimeBaseIndicator = 0;
    m_lastGmPhaseChange.ns = 0;
    m_lastGmPhaseChange.ns_frac = 0;
    m_lastGmFreqChange = 1.0;
    m_txMDSyncSendPtr = std::unique_ptr<MDSyncSend>(new MDSyncSend());
    m_syncReceiptTimeoutTime.ns = 0;
    m_syncReceiptTimeoutTime.ns_frac = 0;

    m_mdSyncSendSM = mdSyncSendSM;
}

PortSyncSyncSend::~PortSyncSyncSend()
{
}

void PortSyncSyncSend::SetMDSync()
{
    if(memcmp(m_lastSourcePortIdentity.clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock)) == 0)
    {
        memcpy(m_txMDSyncSendPtr->sourcePortIdentity.clockIdentity,  m_portGlobal->identity.clockIdentity, sizeof(m_portGlobal->identity.clockIdentity));
        m_txMDSyncSendPtr->sourcePortIdentity.portNumber = m_portGlobal->identity.portNumber;
    }
    else
    {
        memcpy(m_txMDSyncSendPtr->sourcePortIdentity.clockIdentity, m_lastSourcePortIdentity.clockIdentity, sizeof(m_lastSourcePortIdentity.clockIdentity));
        m_txMDSyncSendPtr->sourcePortIdentity.portNumber = m_lastSourcePortIdentity.portNumber;
    }
    m_txMDSyncSendPtr->logMessageInterval = m_portGlobal->currentLogSyncInterval;
    m_txMDSyncSendPtr->preciseOriginTimestamp.sec = m_lastPreciseOriginTimestamp.sec;
    m_txMDSyncSendPtr->preciseOriginTimestamp.ns = m_lastPreciseOriginTimestamp.ns;
    m_txMDSyncSendPtr->rateRatio = m_lastRateRatio;
    m_txMDSyncSendPtr->followUpCorrectionField.ns = m_lastFollowUpCorrectionField.ns;
    m_txMDSyncSendPtr->followUpCorrectionField.ns_frac = m_lastFollowUpCorrectionField.ns_frac;
    m_txMDSyncSendPtr->upstreamTxTime.ns = m_lastUpstreamTxTime.ns;
    m_txMDSyncSendPtr->upstreamTxTime.ns_frac = m_lastUpstreamTxTime.ns_frac;
}

void PortSyncSyncSend::TxMDSync()
{
    m_mdSyncSendSM->SetMDSyncSend(m_txMDSyncSendPtr.get());
}

void PortSyncSyncSend::ProcessSync(PortSyncSync* rcvd)
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

void PortSyncSyncSend::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdPSSync && (!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable)))
    {
        m_state = STATE_TRANSMIT_INIT;
        m_rcvdPSSync = false;
    }
    else
    {
        switch(m_state)
        {
        case STATE_TRANSMIT_INIT:
        case STATE_SYNC_RECEIPT_TIMEOUT:
            if(m_rcvdPSSync && (m_rcvdPSSyncPtr->localPortNumber != m_portGlobal->identity.portNumber) &&
                    m_portGlobal->portEnabled &&  m_portGlobal->pttPortEnabled && m_portGlobal->asCapable &&
                    m_timeAwareSystem->selectedRole[m_portGlobal->identity.portNumber] == PORT_ROLE_MASTER)
            {
                ExecuteSendMDSyncState();
                m_state = STATE_SEND_MD_SYNC;
            }
            break;
        case STATE_SEND_MD_SYNC:
            m_syncReceiptTimeoutTime = m_rcvdPSSyncPtr->syncReceiptTimeoutTime;
            m_state = STATE_SET_SYNC_RECEIPT_TIMEOUT_TIME;
            break;
        case STATE_SET_SYNC_RECEIPT_TIMEOUT_TIME:
            if((((m_rcvdPSSync && (m_timeAwareSystem->GetCurrentTime() - m_lastSyncSentTime >= m_portGlobal->syncInterval * 0.5) &&
                 m_rcvdPSSyncPtr->localPortNumber != m_portGlobal->identity.portNumber)) ||
                ((m_timeAwareSystem->GetCurrentTime() - m_lastSyncSentTime >= m_portGlobal->syncInterval) &&
                   (m_lastRcvdPortNum != m_portGlobal->identity.portNumber))) && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled &&
                    m_portGlobal->asCapable && m_timeAwareSystem->selectedRole[m_portGlobal->identity.portNumber] == PORT_ROLE_MASTER)
            {
                ExecuteSendMDSyncState();
                m_state = STATE_SEND_MD_SYNC;
            }
            else if(m_rcvdPSSync &&
                    (m_timeAwareSystem->GetCurrentTime() - m_lastSyncSentTime < m_portGlobal->syncInterval * 0.5) &&
                    (m_rcvdPSSyncPtr->localPortNumber != m_portGlobal->identity.portNumber)
                    && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable &&
                    m_timeAwareSystem->selectedRole[m_portGlobal->identity.portNumber] == PORT_ROLE_MASTER)
            {
                m_syncReceiptTimeoutTime = m_rcvdPSSyncPtr->syncReceiptTimeoutTime;
            }
            else if(m_timeAwareSystem->GetCurrentTime() >= m_syncReceiptTimeoutTime)
                m_rcvdPSSync = false;
                m_state = STATE_SYNC_RECEIPT_TIMEOUT;
            break;
        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void PortSyncSyncSend::ExecuteSendMDSyncState()
{
    if(m_rcvdPSSync)
    {
        m_lastRcvdPortNum = m_rcvdPSSyncPtr->localPortNumber;
        m_lastPreciseOriginTimestamp = m_rcvdPSSyncPtr->preciseOriginTimestamp;
        m_lastFollowUpCorrectionField = m_rcvdPSSyncPtr->followUpCorrectionField;
        m_lastRateRatio = m_rcvdPSSyncPtr->rateRatio;
        m_lastUpstreamTxTime = m_rcvdPSSyncPtr->upstreamTxTime;
        m_lastGmTimeBaseIndicator = m_rcvdPSSyncPtr->gmTimeBaseIndicator;
        m_lastGmPhaseChange = m_rcvdPSSyncPtr->lastGmPhaseChange;
        m_lastGmFreqChange = m_rcvdPSSyncPtr->lastGmFreqChange;
    }
    m_rcvdPSSync = false;
    m_lastSyncSentTime = m_timeAwareSystem->GetCurrentTime();
    SetMDSync();
    TxMDSync();
}
