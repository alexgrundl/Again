#include "mdsyncreceivesm.h"

MDSyncReceiveSM::MDSyncReceiveSM(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGlobal,
                                 PortSyncSyncReceive* portSyncSyncReceive, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, mdGlobal, networkPort)
{
    m_followUpReceiptTimeoutTime.ns = 0;
    m_followUpReceiptTimeoutTime.ns_frac = 0;
    m_rcvdSync = false;
    m_rcvdFollowUp = false;
    m_rcvdSyncPtr = NULL;
    m_rcvdFollowUpPtr = NULL;
    m_txMDSyncReceivePtr = NULL;
    m_upstreamSyncInterval.ns = 0;
    m_upstreamSyncInterval.ns_frac = 0;

    m_portSyncSyncReceive = portSyncSyncReceive;
}

MDSyncReceiveSM::~MDSyncReceiveSM()
{
    delete m_txMDSyncReceivePtr;
}

MDSyncReceive* MDSyncReceiveSM::SetMDSyncReceive()
{
    PtpMessageFollowUp::FollowUpTLV tlv;
    MDSyncReceive* mdSyncReceivePtr = new MDSyncReceive();

    mdSyncReceivePtr->followUpCorrectionField.ns = m_rcvdFollowUpPtr->GetCorrectionField();
    mdSyncReceivePtr->followUpCorrectionField.ns_frac = 0;
    mdSyncReceivePtr->sourcePortIdentity = m_rcvdSyncPtr->GetSourcePortIdentity();
    mdSyncReceivePtr->logMessageInterval = m_rcvdSyncPtr->GetLogMessageInterval();
    mdSyncReceivePtr->preciseOriginTimestamp = m_rcvdFollowUpPtr->GetPreciseOriginTimestamp();
    m_rcvdFollowUpPtr->GetFollowUpTLV(&tlv);
    mdSyncReceivePtr->rateRatio = tlv.cumulativeScaledRateOffset * pow(2, -41) + 1.0;

    /** upstreamTxTime is set equal to the <syncEventIngressTimestamp> for the most recently received
    Sync message, minus the mean propagation time on the link attached to this port
    (neighborPropDelay, see 10.2.4.7) divided by neighborRateRatio (see 10.2.4.6), minus
    delayAsymmetry (see 10.2.4.8) for this port divided by rateRatio [see e) above]. The
    <syncEventIngressTimestamp> is equal to the timestamp value measured relative to the timestamp
    measurement plane, minus any ingressLatency (see 8.4.3) */

    mdSyncReceivePtr->gmTimeBaseIndicator = tlv.gmTimeBaseIndicator;
    mdSyncReceivePtr->lastGmPhaseChange = tlv.lastGmPhaseChange;
    /* scaledLastGmFreqChange has to be converted to the correct value, probably... */
    //mdSyncReceivePtr->lastGmFreqChange = tlv.scaledLastGmFreqChange;

    return mdSyncReceivePtr;
}

void MDSyncReceiveSM::TxMDSyncReceive(MDSyncReceive* txMDSyncReceivePtr)
{
    m_portSyncSyncReceive->ProcessStruct(txMDSyncReceivePtr);
}

void MDSyncReceiveSM::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdSync && (!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable)))
    {
        m_state = STATE_DISCARD;
        m_rcvdSync = false;
        m_rcvdFollowUp = false;
    }
    else
    {
        switch(m_state)
        {
        case STATE_DISCARD:
        case STATE_WAITING_FOR_SYNC:
            if(m_rcvdSync && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable)
            {
                m_rcvdSync = false;
                m_upstreamSyncInterval.ns = NS_PER_SEC * pow(2, m_rcvdSyncPtr->GetLogMessageInterval());
                m_upstreamSyncInterval.ns_frac = 0;
                m_followUpReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime() + m_upstreamSyncInterval;

                m_state = STATE_WAITING_FOR_FOLLOW_UP;
            }
            break;

        case STATE_WAITING_FOR_FOLLOW_UP:
            if(m_rcvdFollowUp && (m_rcvdFollowUpPtr->GetSequenceID() == m_rcvdSyncPtr->GetSequenceID()))
            {
                m_rcvdFollowUp = false;
                delete m_txMDSyncReceivePtr;
                m_txMDSyncReceivePtr = SetMDSyncReceive();
                TxMDSyncReceive(m_txMDSyncReceivePtr);
                m_state = STATE_WAITING_FOR_SYNC;
            }
            else if(m_timeAwareSystem->GetCurrentTime() >= m_followUpReceiptTimeoutTime)
            {
                UScaledNs time = m_timeAwareSystem->GetCurrentTime();
                printf("Time   : %lu\n", time.ns);
                m_rcvdSync = false;
                m_rcvdFollowUp = false;
                m_state = STATE_DISCARD;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void MDSyncReceiveSM::SetSyncMessage(PtpMessageSync* rcvdSyncPtr)
{
    m_rcvdSyncPtr = rcvdSyncPtr;
    m_rcvdSync = true;
}

void MDSyncReceiveSM::SetFollowUpMessage(PtpMessageFollowUp* rcvdFollowUpPtr)
{
    m_rcvdFollowUpPtr = rcvdFollowUpPtr;
    m_rcvdFollowUp = true;
}
