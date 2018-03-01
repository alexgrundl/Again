#include "mdsyncsendsm.h"

MDSyncSendSM::MDSyncSendSM(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdMDSync = false;
    m_rcvdMDSyncPtr = NULL;
    m_txSyncPtr = NULL;
    m_rcvdMDTimestampReceive = false;
    m_rcvdMDTimestampReceivePtr = NULL;
    m_txFollowUpPtr = NULL;
}

MDSyncSendSM::~MDSyncSendSM()
{
    delete m_txSyncPtr;
    delete m_txFollowUpPtr;
}

PtpMessageSync* MDSyncSendSM::SetSync()
{
    PtpMessageSync* syncPtr = new PtpMessageSync();

    syncPtr->SetCorrectionField(0);
    syncPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    syncPtr->SetSequenceID(m_portGlobal->syncSequenceId);
    syncPtr->SetLogMessageInterval((m_rcvdMDSyncPtr->logMessageInterval));

    return syncPtr;
}

PtpMessageFollowUp* MDSyncSendSM::SetFollowUp()
{
    PtpMessageFollowUp* followUpPtr = new PtpMessageFollowUp();
    ScaledNs correctionScaled;

    correctionScaled = m_rcvdMDSyncPtr->followUpCorrectionField + ((*m_rcvdMDTimestampReceivePtr) - m_rcvdMDSyncPtr->upstreamTxTime) * m_rcvdMDSyncPtr->rateRatio;
    followUpPtr->SetCorrectionField(correctionScaled.ns);
    followUpPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    followUpPtr->SetSequenceID(m_portGlobal->syncSequenceId);
    followUpPtr->SetLogMessageInterval(m_rcvdMDSyncPtr->logMessageInterval);
    followUpPtr->SetPreciseOriginTimestamp(m_rcvdMDSyncPtr->preciseOriginTimestamp);

    followUpPtr->SetCumulativeScaledRateOffset((m_rcvdMDSyncPtr->rateRatio - 1.0) * pow(2, 41));
    followUpPtr->SetGmTimeBaseIndicator(m_rcvdMDSyncPtr->gmTimeBaseIndicator);
    followUpPtr->SetLastGmPhaseChange(m_rcvdMDSyncPtr->lastGmPhaseChange);
    followUpPtr->SetScaledLastGmFreqChange(m_rcvdMDSyncPtr->lastGmFreqChange * pow(2, 41));

    return followUpPtr;
}


void MDSyncSendSM::TxFollowUp(PtpMessageFollowUp* txFollowUpPtr)
{

}

void MDSyncSendSM::TxSync(PtpMessageSync *txSyncPtr)
{

}

void MDSyncSendSM::SetMDSyncSend(MDSyncSend* rcvdMDSyncPtr)
{
    m_rcvdMDSyncPtr = rcvdMDSyncPtr;
    m_rcvdMDSync = true;
}

void MDSyncSendSM::SetMDTimestampReceive(MDTimestampReceive* rcvdMDTimestampReceivePtr)
{
    m_rcvdMDTimestampReceivePtr = rcvdMDTimestampReceivePtr;
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
                delete m_txSyncPtr;
                m_txSyncPtr = SetSync();
                TxSync(m_txSyncPtr);
                m_portGlobal->syncSequenceId += 1;
                m_state = STATE_SEND_SYNC;
            }
            break;

        case STATE_SEND_SYNC:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                delete m_txFollowUpPtr;
                m_txFollowUpPtr = SetFollowUp();
                TxFollowUp(m_txFollowUpPtr);
                m_state = STATE_SEND_FOLLOW_UP;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

