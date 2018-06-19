#include "mdsyncsendsm.h"

MDSyncSendSM::MDSyncSendSM(TimeAwareSystem* timeAwareSystem, SystemPort* port, INetPort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdMDSync = false;
    m_rcvdMDSyncPtr = new MDSyncSend();
    m_txSyncPtr = new PtpMessageSync();
    m_rcvdMDTimestampReceive = false;
//    m_rcvdMDTimestampReceivePtr = new MDTimestampReceive();
    m_txFollowUpPtr = new PtpMessageFollowUp();
}

MDSyncSendSM::~MDSyncSendSM()
{
    delete m_rcvdMDSyncPtr;
    delete m_txSyncPtr;
//    delete m_rcvdMDTimestampReceivePtr;
    delete m_txFollowUpPtr;
}

void MDSyncSendSM::SetSync()
{
    m_txSyncPtr->SetCorrectionField(0);
    m_txSyncPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    m_txSyncPtr->SetSequenceID(m_systemPort->GetSyncSequenceId());
    m_txSyncPtr->SetLogMessageInterval((m_rcvdMDSyncPtr->logMessageInterval));

    m_txSyncPtr->SetDomainNumber(m_rcvdMDSyncPtr->domain);
}

void MDSyncSendSM::SetFollowUp()
{
    ScaledNs correctionScaled;
    Timestamp preciseOriginTimestamp = m_rcvdMDSyncPtr->preciseOriginTimestamp;
    UScaledNs sendTime = m_txSyncPtr->GetSendTime();
    UScaledNs upstreamTxTime = m_rcvdMDSyncPtr->upstreamTxTime;

    /* If we are master (Port Role 0 == Slave) calculate the GPS time from the device time. */
    if(m_timeAwareSystem->GetSelectedRole(0) == PORT_ROLE_SLAVE && m_timeAwareSystem->GetTimeSource() == CLOCK_TIME_SOURCE_GPS)
    {
        uint64_t gpsTime;

        if(m_timeAwareSystem->GetGPSTime(preciseOriginTimestamp.sec * NS_PER_SEC + preciseOriginTimestamp.ns, &gpsTime))
        {
            preciseOriginTimestamp.sec = gpsTime / NS_PER_SEC;
            preciseOriginTimestamp.ns = gpsTime % NS_PER_SEC;

            m_timeAwareSystem->GetGPSTime(sendTime.ns, &gpsTime);
            sendTime.ns = gpsTime;
            m_timeAwareSystem->GetGPSTime(upstreamTxTime.ns, &gpsTime);
            upstreamTxTime.ns = gpsTime;
        }
        else
            m_timeAwareSystem->SetTimeSource(CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR);

        //printf("preciseOriginTimestamp: %lu\n", preciseOriginTimestamp.sec * NS_PER_SEC + preciseOriginTimestamp.ns);
    }

    //printf("m_rcvdMDSyncPtr->followUpCorrectionField: %lu\n", m_rcvdMDSyncPtr->followUpCorrectionField.ns);
    correctionScaled = m_rcvdMDSyncPtr->followUpCorrectionField + (sendTime - upstreamTxTime) * m_rcvdMDSyncPtr->rateRatio;

    m_txFollowUpPtr->SetCorrectionField((correctionScaled.ns << 16) + correctionScaled.ns_frac);
    m_txFollowUpPtr->SetSourcePortIdentity(&m_rcvdMDSyncPtr->sourcePortIdentity);
    m_txFollowUpPtr->SetSequenceID(m_txSyncPtr->GetSequenceID());
    m_txFollowUpPtr->SetLogMessageInterval(m_rcvdMDSyncPtr->logMessageInterval);
    m_txFollowUpPtr->SetPreciseOriginTimestamp(preciseOriginTimestamp);

    m_txFollowUpPtr->SetCumulativeScaledRateOffset((m_rcvdMDSyncPtr->rateRatio - 1.0) * pow(2, 41));
    m_txFollowUpPtr->SetGmTimeBaseIndicator(m_rcvdMDSyncPtr->gmTimeBaseIndicator);
    m_txFollowUpPtr->SetLastGmPhaseChange(m_rcvdMDSyncPtr->lastGmPhaseChange);
    m_txFollowUpPtr->SetScaledLastGmFreqChange(m_rcvdMDSyncPtr->lastGmFreqChange * pow(2, 41));

    m_txFollowUpPtr->SetDomainNumber(m_rcvdMDSyncPtr->domain);
}


void MDSyncSendSM::TxFollowUp()
{
    m_networkPort->SendGenericMessage(m_txFollowUpPtr);
}

void MDSyncSendSM::TxSync()
{
    m_txSyncPtr->SetSendTime(m_networkPort->SendEventMessage(m_txSyncPtr));

    m_rcvdMDTimestampReceive = m_txSyncPtr->GetSendTime().ns > 0;
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
    m_rcvdMDSyncPtr->domain = rcvdMDSyncPtr->domain;
    m_rcvdMDSync = true;
}

void MDSyncSendSM::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdMDSync && (!m_systemPort->IsPortEnabled() || !m_systemPort->IsPttPortEnabled() || !m_systemPort->GetAsCapable())))
    {
        m_rcvdMDSync = false;
        m_rcvdMDTimestampReceive = false;
        m_systemPort->SetSyncSequenceId(rand() % 65536);
        m_state = STATE_INITIALIZING;
    }
    else
    {
        switch(m_state)
        {
        case STATE_INITIALIZING:
        case STATE_SEND_FOLLOW_UP:
            if(m_rcvdMDSync && m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled() && m_systemPort->GetAsCapable())
            {
                m_rcvdMDSync = false;
                SetSync();
                TxSync();
                m_systemPort->IncreaseSyncSequenceId();
                m_state = STATE_SEND_SYNC;
            }
            break;

        case STATE_SEND_SYNC:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                SetFollowUp();
                TxFollowUp();
            }
            else
                logerror("Couldn't get transmit timestamp of Sync message.");
            m_state = STATE_SEND_FOLLOW_UP;
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
