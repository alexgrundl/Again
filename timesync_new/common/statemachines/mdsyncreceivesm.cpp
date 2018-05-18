#include "mdsyncreceivesm.h"

MDSyncReceiveSM::MDSyncReceiveSM(TimeAwareSystem* timeAwareSystem, SystemPort* port, PortSyncSyncReceive* portSyncSyncReceive,
                                 INetPort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_followUpReceiptTimeoutTime.ns = 0;
    m_followUpReceiptTimeoutTime.ns_frac = 0;
    m_rcvdSync = false;
    m_rcvdFollowUp = false;
    m_rcvdSyncPtr = new PtpMessageSync();
    m_rcvdFollowUpPtr = new PtpMessageFollowUp();
    m_txMDSyncReceivePtr = new MDSyncReceive();
    m_upstreamSyncInterval.ns = 0;
    m_upstreamSyncInterval.ns_frac = 0;

    m_portSyncSyncReceive = portSyncSyncReceive;
}

MDSyncReceiveSM::~MDSyncReceiveSM()
{
    delete m_rcvdSyncPtr;
    delete m_rcvdFollowUpPtr;
    delete m_txMDSyncReceivePtr;
}

void MDSyncReceiveSM::SetMDSyncReceive()
{
    PtpMessageFollowUp::FollowUpTLV tlv;

    m_txMDSyncReceivePtr->followUpCorrectionField.ns = m_rcvdFollowUpPtr->GetCorrectionField() >> 16;
    m_txMDSyncReceivePtr->followUpCorrectionField.ns_frac = m_rcvdFollowUpPtr->GetCorrectionField() & 0xFFFF;
    m_txMDSyncReceivePtr->sourcePortIdentity = m_rcvdSyncPtr->GetSourcePortIdentity();
    m_txMDSyncReceivePtr->logMessageInterval = m_rcvdSyncPtr->GetLogMessageInterval();
    m_txMDSyncReceivePtr->preciseOriginTimestamp = m_rcvdFollowUpPtr->GetPreciseOriginTimestamp();
    m_rcvdFollowUpPtr->GetFollowUpTLV(&tlv);
    m_txMDSyncReceivePtr->rateRatio = tlv.cumulativeScaledRateOffset * pow(2, -41) + 1.0;

    m_txMDSyncReceivePtr->upstreamTxTime = m_rcvdSyncPtr->GetReceiveTime() - (m_systemPort->GetNeighborPropDelay() / m_systemPort->GetNeighborRateRatio());
    /** upstreamTxTime is set equal to the <syncEventIngressTimestamp> for the most recently received
    Sync message, minus the mean propagation time on the link attached to this port
    (neighborPropDelay, see 10.2.4.7) divided by neighborRateRatio (see 10.2.4.6), minus
    delayAsymmetry (see 10.2.4.8) for this port divided by rateRatio [see e) above]. The
    <syncEventIngressTimestamp> is equal to the timestamp value measured relative to the timestamp
    measurement plane, minus any ingressLatency (see 8.4.3) */

    m_txMDSyncReceivePtr->gmTimeBaseIndicator = tlv.gmTimeBaseIndicator;
    m_txMDSyncReceivePtr->lastGmPhaseChange = tlv.lastGmPhaseChange;
    /* scaledLastGmFreqChange has to be converted to the correct value, probably... */
    m_txMDSyncReceivePtr->lastGmFreqChange = tlv.scaledLastGmFreqChange / pow(2, 41);

    m_txMDSyncReceivePtr->domain = m_rcvdFollowUpPtr->GetDomainNumber();
}

void MDSyncReceiveSM::TxMDSyncReceive()
{
    m_portSyncSyncReceive->ProcessSync(m_txMDSyncReceivePtr);
}

void MDSyncReceiveSM::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdSync && (!m_systemPort->IsPortEnabled() || !m_systemPort->IsPttPortEnabled() || !m_systemPort->GetAsCapable())))
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
            if(m_rcvdSync && m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled() && m_systemPort->GetAsCapable())
            {
                m_rcvdSync = false;
                m_upstreamSyncInterval.ns = NS_PER_SEC * pow(2, m_rcvdSyncPtr->GetLogMessageInterval());
                m_upstreamSyncInterval.ns_frac = 0;
                m_followUpReceiptTimeoutTime = m_timeAwareSystem->ReadCurrentTime() + m_upstreamSyncInterval;

                m_state = STATE_WAITING_FOR_FOLLOW_UP;
            }
            break;

        case STATE_WAITING_FOR_FOLLOW_UP:
            if(m_rcvdFollowUp && (m_rcvdFollowUpPtr->GetSequenceID() == m_rcvdSyncPtr->GetSequenceID()))
            {
                m_rcvdFollowUp = false;
                SetMDSyncReceive();
                TxMDSyncReceive();
                m_state = STATE_WAITING_FOR_SYNC;
            }
            else if(m_timeAwareSystem->ReadCurrentTime() >= m_followUpReceiptTimeoutTime)
            {
                //UScaledNs time = m_timeAwareSystem->GetCurrentTime();
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

void MDSyncReceiveSM::SetSyncMessage(ReceivePackage *package)
{
    m_rcvdSyncPtr->ParsePackage(package->GetBuffer());
    m_rcvdSyncPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdSync = true;
}

void MDSyncReceiveSM::SetFollowUpMessage(ReceivePackage *package)
{
    m_rcvdFollowUpPtr->ParsePackage(package->GetBuffer());
    m_rcvdFollowUpPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdFollowUp = true;
}
