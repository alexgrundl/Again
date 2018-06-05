#include "portsyncsyncsend.h"

PortSyncSyncSend::PortSyncSyncSend(TimeAwareSystem* timeAwareSystem, SystemPort* port, MDSyncSendSM *mdSyncSendSM) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_rcvdPSSync = false;
    m_rcvdPSSyncPtr = new PortSyncSync();
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
    m_txMDSyncSendPtr = new MDSyncSend();
    m_syncReceiptTimeoutTime.ns = 0;
    m_syncReceiptTimeoutTime.ns_frac = 0;
    m_lastDomain = 0;

    m_mdSyncSendSM = mdSyncSendSM;
}

PortSyncSyncSend::~PortSyncSyncSend()
{
    delete m_txMDSyncSendPtr;
    delete m_rcvdPSSyncPtr;
}

void PortSyncSyncSend::SetMDSync()
{
    /*  The standard says:
        "sourcePortIdentity is set to the portIdentity of this port if the clockIdentity member of
        lastSourcePortIdentity (see 10.2.11.1.3) is equal to thisClock (see 10.2.3.22); otherwise, it is set to
        lastSourcePortIdentity"
        But if we do that and are forwarding a Sync message received from another system we'd have the source port identity of
        the port who received this Sync message. We do not want this because:

        - If another system, where our daemon is running, is connected to this port the "PortSyncSyncReceive" state machine
        of this system won't forward this message to the "SiteSyncSync" state machine because the port number of the messages
        source port identity wouldn't equal the port identity of the grandmaster port priority (set in "PortRoleSelection").
        Thus, no synchronization would be done.

        - The current GPTP - Windows - Daemon doesn't do this, too. So, as there haven't been any complaints about this
        we want the same behavior.
    */
    if(true)//memcmp(m_lastSourcePortIdentity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH) == 0)
    {
        m_txMDSyncSendPtr->sourcePortIdentity = m_systemPort->GetIdentity();
    }
    else
    {
        memcpy(m_txMDSyncSendPtr->sourcePortIdentity.clockIdentity, m_lastSourcePortIdentity.clockIdentity, CLOCK_ID_LENGTH);
        m_txMDSyncSendPtr->sourcePortIdentity.portNumber = m_lastSourcePortIdentity.portNumber;
    }
    m_txMDSyncSendPtr->logMessageInterval = m_systemPort->GetCurrentLogSyncInterval();
    m_txMDSyncSendPtr->preciseOriginTimestamp.sec = m_lastPreciseOriginTimestamp.sec;
    m_txMDSyncSendPtr->preciseOriginTimestamp.ns = m_lastPreciseOriginTimestamp.ns;
    m_txMDSyncSendPtr->rateRatio = m_lastRateRatio;
    m_txMDSyncSendPtr->followUpCorrectionField.ns = m_lastFollowUpCorrectionField.ns;
    m_txMDSyncSendPtr->followUpCorrectionField.ns_frac = m_lastFollowUpCorrectionField.ns_frac;
    m_txMDSyncSendPtr->upstreamTxTime.ns = m_lastUpstreamTxTime.ns;
    m_txMDSyncSendPtr->upstreamTxTime.ns_frac = m_lastUpstreamTxTime.ns_frac;
    m_txMDSyncSendPtr->domain = m_lastDomain;
}

void PortSyncSyncSend::TxMDSync()
{
    m_mdSyncSendSM->SetMDSyncSend(m_txMDSyncSendPtr);
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
    m_rcvdPSSyncPtr->domain = rcvd->domain;
    m_rcvdPSSync = true;
}

void PortSyncSyncSend::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdPSSync && (!m_systemPort->IsPortEnabled() || !m_systemPort->IsPttPortEnabled() || !m_systemPort->GetAsCapable())))
    {
        m_state = STATE_TRANSMIT_INIT;
        m_rcvdPSSync = false;
    }
    else
    {
        uint16_t systemPortNumber = m_systemPort->GetIdentity().portNumber;
        switch(m_state)
        {
        case STATE_TRANSMIT_INIT:
        case STATE_SYNC_RECEIPT_TIMEOUT:
            if(m_rcvdPSSync && (m_rcvdPSSyncPtr->localPortNumber != systemPortNumber) &&
                    m_systemPort->IsPortEnabled() &&  m_systemPort->IsPttPortEnabled() && m_systemPort->GetAsCapable() &&
                    m_timeAwareSystem->GetSelectedRole(systemPortNumber) == PORT_ROLE_MASTER)
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
            if((((m_rcvdPSSync && (m_timeAwareSystem->ReadCurrentTime() - m_lastSyncSentTime >= m_systemPort->GetSyncInterval() * 0.5) &&
                 m_rcvdPSSyncPtr->localPortNumber != systemPortNumber)) ||
                ((m_timeAwareSystem->ReadCurrentTime() - m_lastSyncSentTime >= m_systemPort->GetSyncInterval()) &&
                   (m_lastRcvdPortNum != systemPortNumber))) && m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled() &&
                    m_systemPort->GetAsCapable() && m_timeAwareSystem->GetSelectedRole(systemPortNumber) == PORT_ROLE_MASTER)
            {
                ExecuteSendMDSyncState();
                m_state = STATE_SEND_MD_SYNC;
            }
            else if(m_rcvdPSSync &&
                    (m_timeAwareSystem->ReadCurrentTime() - m_lastSyncSentTime < m_systemPort->GetSyncInterval() * 0.5) &&
                    (m_rcvdPSSyncPtr->localPortNumber != systemPortNumber)
                    && m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled() && m_systemPort->GetAsCapable() &&
                    m_timeAwareSystem->GetSelectedRole(systemPortNumber) == PORT_ROLE_MASTER)
            {
                m_syncReceiptTimeoutTime = m_rcvdPSSyncPtr->syncReceiptTimeoutTime;
            }
            else if(m_timeAwareSystem->ReadCurrentTime() >= m_syncReceiptTimeoutTime)
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

        //missing in the standard...
        m_lastSourcePortIdentity = m_rcvdPSSyncPtr->sourcePortIdentity;
        m_lastDomain = m_rcvdPSSyncPtr->domain;
    }
    m_rcvdPSSync = false;
    m_lastSyncSentTime = m_timeAwareSystem->ReadCurrentTime();
    SetMDSync();
    TxMDSync();
}
