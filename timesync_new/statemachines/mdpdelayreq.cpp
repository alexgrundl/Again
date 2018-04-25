#include "mdpdelayreq.h"

MDPdelayReq::MDPdelayReq(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_pdelayIntervalTimer.ns = 0;
    m_pdelayIntervalTimer.ns_frac = 0;
    m_rcvdPdelayResp = false;
    m_rcvdPdelayRespPtr = new PtpMessagePDelayResp();
    m_rcvdPdelayRespFollowUp = false;
    m_rcvdPdelayRespFollowUpPtr = new PtpMessagePDelayRespFollowUp();
    m_txPdelayReqPtr = new PtpMessagePDelayReq();
    m_rcvdMDTimestampReceive = false;
    m_pdelayReqSequenceId = 0;
    m_initPdelayRespReceived = false;
    m_lostResponses = 0;
    m_neighborRateRatioValid = false;

    m_pdelayRespTime.ns = 0;
    m_pdelayRespTime.ns_frac = 0;
    m_pdelayRespFollowUpCorrTime.ns = 0;
    m_pdelayRespFollowUpCorrTime.ns_frac = 0;
}

MDPdelayReq::~MDPdelayReq()
{
    delete m_rcvdPdelayRespPtr;
    delete m_rcvdPdelayRespFollowUpPtr;
    delete m_txPdelayReqPtr;
}

void MDPdelayReq::SetPdelayReq()
{
    PortIdentity identity;
    identity.portNumber = m_portGlobal->identity.portNumber;

    PtpMessageBase::GetClockIdentity(m_networkPort->GetMAC(), identity.clockIdentity);
    m_txPdelayReqPtr->SetSourcePortIdentity(&identity);
    /* 1) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity
        (see 8.5.2),

       3) remaining parameters are set as specified in 11.4.2 and 11.4.5. */

    m_txPdelayReqPtr->SetSequenceID(m_pdelayReqSequenceId);
}

void MDPdelayReq::TxPdelayReq()
{
    m_txPdelayReqPtr->SetSendTime(m_networkPort->SendEventMessage(m_txPdelayReqPtr));
    m_rcvdMDTimestampReceive = true;
}

double MDPdelayReq::ComputePdelayRateRatio()
{
    double pdelayRateRatio = 1.0;
    UScaledNs currentResponderEventTimeCorrected;

    currentResponderEventTimeCorrected.ns = m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp().sec * NS_PER_SEC +
            m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp().ns;
    currentResponderEventTimeCorrected.ns_frac = m_rcvdPdelayRespFollowUpPtr->GetCorrectionField();
    if(m_pdelayRespTime.ns > 0 && m_pdelayRespFollowUpCorrTime.ns > 0)
    {
        pdelayRateRatio = (currentResponderEventTimeCorrected - m_pdelayRespFollowUpCorrTime) /
                (m_rcvdPdelayRespPtr->GetReceiveTime() - m_pdelayRespTime);
        m_neighborRateRatioValid = true;

        //printf("neighborRateRatio port %i: %0.9f\n", m_portGlobal->identity.portNumber, pdelayRateRatio);
    }
    else
        m_neighborRateRatioValid = false;

    m_pdelayRespTime = m_rcvdPdelayRespPtr->GetReceiveTime();
    m_pdelayRespFollowUpCorrTime = currentResponderEventTimeCorrected;

    return pdelayRateRatio;
}

UScaledNs MDPdelayReq::ComputePropTime()
{
    UScaledNs propTime;
    Timestamp tsPropTime;
    Timestamp t1, t2, t3, t4;
    Timestamp localDelta, remoteDelta;

    t1 = m_txPdelayReqPtr->GetSendTime();
    t2 = m_rcvdPdelayRespPtr->GetRequestReceiptTimestamp();
    t3 = m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp();
    t4 = m_rcvdPdelayRespPtr->GetReceiveTime();
    localDelta = t4 - t1;
    remoteDelta = t3 - t2;

    if(localDelta >= remoteDelta)
        tsPropTime = (localDelta - remoteDelta) / 2;
    else
    {
        tsPropTime.sec = 0;
        tsPropTime.ns = 0;
    }

    if(m_neighborRateRatioValid)
        tsPropTime *= m_portGlobal->neighborRateRatio;

    propTime.ns = tsPropTime.sec * NS_PER_SEC + tsPropTime.ns;
    propTime.ns_frac = 0;

    //printf("PropTime port %i: %lu\n", m_portGlobal->identity.portNumber, propTime.ns);

    return propTime;
}

void MDPdelayReq::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || !m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled)
        m_state = STATE_NOT_ENABLED;
    else
    {
        switch(m_state)
        {
        case STATE_NOT_ENABLED:
            if(m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled)
            {
                if(m_timeAwareSystem->GetDomain() == TimeAwareSystem::GetDomainToMeasurePDelay())
                {
                    m_initPdelayRespReceived = false;
                    //Hmmmmmmmmmmmmmmmmmmmmm.....
                    /* m_pdelayRateRatio = 1.0; */
                    m_rcvdMDTimestampReceive = false;
                    srand(time(NULL));
                    m_pdelayReqSequenceId = rand() % 65536;
                    SetPdelayReq();
                    TxPdelayReq();
                    m_pdelayIntervalTimer = m_timeAwareSystem->ReadCurrentTime();
                    m_lostResponses = 0;
                    m_portGlobal->isMeasuringDelay = false;
                    m_portGlobal->asCapable = false;
                    m_networkPort->SetASCapable(false);
                    m_state = STATE_INITIAL_SEND_PDELAY_REQ;
                }
                else
                {
                    m_portGlobal->asCapable = m_networkPort->GetASCapable();
                    m_portGlobal->neighborPropDelay = {m_networkPort->GetPDelay(), 0};
                    m_portGlobal->neighborRateRatio = m_networkPort->GetNeighborRatio();
                }
            }
            break;

        case STATE_INITIAL_SEND_PDELAY_REQ:
        case STATE_SEND_PDELAY_REQ:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP:
            uint8_t portClockIdentity[CLOCK_ID_LENGTH];
            PtpMessageBase::GetClockIdentity(m_networkPort->GetMAC(), portClockIdentity);
            if(m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())
                    && memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity().clockIdentity, portClockIdentity, CLOCK_ID_LENGTH) == 0 &&
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity().portNumber == m_portGlobal->identity.portNumber))
            {
                m_rcvdPdelayResp = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP;
            }
            else if((m_timeAwareSystem->ReadCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp &&
                    (memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity().clockIdentity, portClockIdentity, CLOCK_ID_LENGTH) != 0 ||
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity().portNumber != m_portGlobal->identity.portNumber) ||
                    (m_rcvdPdelayRespPtr->GetSequenceID() != m_txPdelayReqPtr->GetSequenceID()))))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP:
            if(m_rcvdPdelayRespFollowUp && m_rcvdPdelayRespFollowUpPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID() &&
                    memcmp(m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().clockIdentity, m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity,
                           CLOCK_ID_LENGTH) == 0 &&
                    m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().portNumber == m_rcvdPdelayRespPtr->GetSourcePortIdentity().portNumber)
            {
                m_rcvdPdelayRespFollowUp = false;
                if (m_portGlobal->computeNeighborRateRatio)
                {
                    m_portGlobal->neighborRateRatio = ComputePdelayRateRatio();
                    m_networkPort->SetNeighborRatio(m_portGlobal->neighborRateRatio);
                }
                if (m_portGlobal->computeNeighborPropDelay)
                {
                    m_portGlobal->neighborPropDelay = ComputePropTime();
                    m_networkPort->SetPDelay(m_portGlobal->neighborPropDelay.ns);
                }
                m_lostResponses = 0;
                m_portGlobal->isMeasuringDelay = true;
                if ((m_portGlobal->neighborPropDelay <= m_portGlobal->neighborPropDelayThresh) &&
                        memcmp(m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH) != 0
                        && m_neighborRateRatioValid)
                {
                    m_portGlobal->asCapable = true;
                    m_networkPort->SetASCapable(true);
                }
                else
                {
                    m_portGlobal->asCapable = false;
                    m_networkPort->SetASCapable(false);
                }
                m_state = STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER;
            }
            else if((m_timeAwareSystem->ReadCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER:
            if(m_timeAwareSystem->ReadCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval)
            {
                ExecuteSendPDelayReqState();
                m_state = STATE_SEND_PDELAY_REQ;
            }
            break;

        case STATE_RESET:
            ExecuteSendPDelayReqState();
            m_state = STATE_SEND_PDELAY_REQ;
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void MDPdelayReq::ExecuteResetState()
{
    m_initPdelayRespReceived = false;
    if (m_lostResponses <= m_portGlobal->allowedLostResponses)
    m_lostResponses += 1;
    else
    {
        m_portGlobal->isMeasuringDelay = false;
        m_portGlobal->asCapable = false;
        m_networkPort->SetASCapable(false);
    }
}

void MDPdelayReq::ExecuteSendPDelayReqState()
{
    m_pdelayReqSequenceId += 1;
    SetPdelayReq();
    TxPdelayReq();
    m_pdelayIntervalTimer = m_timeAwareSystem->ReadCurrentTime();
}


void MDPdelayReq::SetPDelayResponse(IReceivePackage* package)
{
    m_rcvdPdelayRespPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayRespPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayResp = true;
}

void MDPdelayReq::SetPDelayResponseFollowUp(IReceivePackage* package)
{
    m_rcvdPdelayRespFollowUpPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayRespFollowUpPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayRespFollowUp = true;
}
