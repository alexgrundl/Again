#include "mdsyncsendsm.h"

MDSyncSendSM::MDSyncSendSM(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdMDSync = false;
    m_rcvdMDSyncPtr = std::unique_ptr<MDSyncSend>(new MDSyncSend());
    m_txSyncPtr = std::unique_ptr<PtpMessageSync>(new PtpMessageSync());
    m_rcvdMDTimestampReceive = false;
    m_rcvdMDTimestampReceivePtr = std::unique_ptr<MDTimestampReceive>(new MDTimestampReceive());
    m_txFollowUpPtr = std::unique_ptr<PtpMessageFollowUp>(new PtpMessageFollowUp());
}

MDSyncSendSM::~MDSyncSendSM()
{
}

void MDSyncSendSM::SetSync()
{
    m_txSyncPtr->SetCorrectionField(0);
    m_txSyncPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    m_txSyncPtr->SetSequenceID(m_portGlobal->syncSequenceId);
    m_txSyncPtr->SetLogMessageInterval((m_rcvdMDSyncPtr->logMessageInterval));
}

void MDSyncSendSM::SetFollowUp()
{
    ScaledNs correctionScaled;

    correctionScaled = m_rcvdMDSyncPtr->followUpCorrectionField + ((*m_rcvdMDTimestampReceivePtr) - m_rcvdMDSyncPtr->upstreamTxTime) * m_rcvdMDSyncPtr->rateRatio;
    m_txFollowUpPtr->SetCorrectionField(correctionScaled.ns);
    m_txFollowUpPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    m_txFollowUpPtr->SetSequenceID(m_portGlobal->syncSequenceId);
    m_txFollowUpPtr->SetLogMessageInterval(m_rcvdMDSyncPtr->logMessageInterval);
    m_txFollowUpPtr->SetPreciseOriginTimestamp(m_rcvdMDSyncPtr->preciseOriginTimestamp);

    m_txFollowUpPtr->SetCumulativeScaledRateOffset((m_rcvdMDSyncPtr->rateRatio - 1.0) * pow(2, 41));
    m_txFollowUpPtr->SetGmTimeBaseIndicator(m_rcvdMDSyncPtr->gmTimeBaseIndicator);
    m_txFollowUpPtr->SetLastGmPhaseChange(m_rcvdMDSyncPtr->lastGmPhaseChange);
    m_txFollowUpPtr->SetScaledLastGmFreqChange(m_rcvdMDSyncPtr->lastGmFreqChange * pow(2, 41));
}


void MDSyncSendSM::TxFollowUp()
{

}

void MDSyncSendSM::TxSync()
{

}

void MDSyncSendSM::SetMDSyncSend(MDSyncSend* rcvdMDSyncPtr)
{
    m_rcvdMDSyncPtr->followUpCorrectionField = rcvdMDSyncPtr->followUpCorrectionField;
    m_rcvdMDSyncPtr->gmTimeBaseIndicator = rcvdMDSyncPtr->gmTimeBaseIndicator;
    m_rcvdMDSyncPtr->lastGmFreqChange = rcvdMDSyncPtr->lastGmFreqChange;
    m_rcvdMDSyncPtr->lastGmPhaseChange = rcvdMDSyncPtr->lastGmPhaseChange;
    m_rcvdMDSyncPtr->logMessageInterval = rcvdMDSyncPtr->logMessageInterval;
    m_rcvdMDSyncPtr->preciseOriginTimestamp = rcvdMDSyncPtr->preciseOriginTimestamp;
    m_rcvdMDSyncPtr->rateRatio = rcvdMDSyncPtr->rateRatio;
    m_rcvdMDSyncPtr->sourcePortIdentity = rcvdMDSyncPtr->sourcePortIdentity;
    m_rcvdMDSyncPtr->upstreamTxTime = rcvdMDSyncPtr->upstreamTxTime;
    m_rcvdMDSync = true;
}

void MDSyncSendSM::SetMDTimestampReceive(MDTimestampReceive* rcvdMDTimestampReceivePtr)
{
    m_rcvdMDTimestampReceivePtr->ns = rcvdMDTimestampReceivePtr->ns;
    m_rcvdMDTimestampReceivePtr->ns_frac = rcvdMDTimestampReceivePtr->ns_frac;
    m_rcvdMDTimestampReceive = true;
}

void MDSyncSendSM::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdMDSync && (!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable)))
    {
        m_rcvdMDSync = false;
        m_rcvdMDTimestampReceive = false;
        m_portGlobal->syncSequenceId = rand() % 65536;
        m_state = STATE_INITIALIZING;
    }
    else
    {
        switch(m_state)
        {
        case STATE_INITIALIZING:
        case STATE_SEND_FOLLOW_UP:
            if(m_rcvdMDSync && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable)
            {
                m_rcvdMDSync = false;
                SetSync();
                TxSync();
                m_portGlobal->syncSequenceId += 1;
                m_state = STATE_SEND_SYNC;
            }
            break;

        case STATE_SEND_SYNC:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                SetFollowUp();
                TxFollowUp();
                m_state = STATE_SEND_FOLLOW_UP;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

