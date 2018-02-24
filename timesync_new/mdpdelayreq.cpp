#include "mdpdelayreq.h"

MDPdelayReq::MDPdelayReq(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGlobal) :
    StateMachineBaseMD(timeAwareSystem, port, mdGlobal)
{
    m_pdelayIntervalTimer.ns = 0;
    m_pdelayIntervalTimer.ns_frac = 0;
    m_rcvdPdelayResp = false;
    m_rcvdPdelayRespPtr = NULL;
    m_rcvdPdelayRespFollowUp = false;
    m_rcvdPdelayRespFollowUpPtr = NULL;
    m_txPdelayReqPtr = NULL;
    m_rcvdMDTimestampReceive = false;
    m_pdelayReqSequenceId = 0;
    m_initPdelayRespReceived = false;
    m_lostResponses = 0;
    m_neighborRateRatioValid = false;
}

MDPdelayReq::~MDPdelayReq()
{
    delete m_txPdelayReqPtr;
}

PtpMessagePDelayReq* MDPdelayReq::SetPdelayReq()
{
    PtpMessagePDelayReq* pdelayReqPtr = new PtpMessagePDelayReq();

    /* 1) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity
        (see 8.5.2),

       3) remaining parameters are set as specified in 11.4.2 and 11.4.5. */

    pdelayReqPtr->SetSequenceID(m_pdelayReqSequenceId);

    return pdelayReqPtr;
}

void MDPdelayReq::TxPdelayReq(PtpMessagePDelayReq* txPdelayReqPtr)
{
    /* transmits a Pdelay_Req message from the MD entity, containing the parameters in the structure pointed to by txPdelayReqPtr. */
}

double MDPdelayReq::ComputePdelayRateRatio()
{
    double pdelayRateRatio = 1.0;

    return pdelayRateRatio;
}

UScaledNs MDPdelayReq::ComputePropTime()
{
     UScaledNs propTime = {0, 0};

     return propTime;
}

void MDPdelayReq::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled)
        m_state = STATE_NOT_ENABLED;
    else
    {
        switch(m_state)
        {
        case STATE_NOT_ENABLED:
            if(m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled)
            {
                m_initPdelayRespReceived = false;
                //Hmmmmmmmmmmmmmmmmmmmmm.....
                /* m_pdelayRateRatio = 1.0; */
                m_rcvdMDTimestampReceive = false;
                m_pdelayReqSequenceId = rand() % 65536;
                m_txPdelayReqPtr = SetPdelayReq();
                delete m_txPdelayReqPtr;
                TxPdelayReq(m_txPdelayReqPtr);
                m_pdelayIntervalTimer = m_timeAwareSystem->GetCurrentTime();
                m_lostResponses = 0;
                m_mdGlobal->isMeasuringDelay = false;
                m_portGlobal->asCapable = false;
                m_state = STATE_INITIAL_SEND_PDELAY_REQ;
            }
            break;

        case STATE_INITIAL_SEND_PDELAY_REQ:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP:
            if(m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())
                    && memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock)) == 0 &&
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->portNumber == m_portGlobal->thisPort))
            {
                m_rcvdPdelayResp = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp &&
                    (memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock)) != 0 ||
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->portNumber != m_portGlobal->thisPort) ||
                    (m_rcvdPdelayRespPtr->GetSequenceID() != m_txPdelayReqPtr->GetSequenceID()))))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP:
            if(m_rcvdPdelayRespFollowUp && m_rcvdPdelayRespFollowUpPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID() &&
                    memcmp(m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().clockIdentity, m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity,
                           sizeof(m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity)) == 0 &&
                    m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().portNumber == m_rcvdPdelayRespPtr->GetSourcePortIdentity().portNumber)
            {
                m_rcvdPdelayRespFollowUp = false;
                if (m_portGlobal->computeNeighborRateRatio)
                m_portGlobal->neighborRateRatio = ComputePdelayRateRatio();
                if (m_portGlobal->computeNeighborPropDelay)
                m_portGlobal->neighborPropDelay = ComputePropTime();
                m_lostResponses = 0;
                m_mdGlobal->isMeasuringDelay = true;
                if ((m_portGlobal->neighborPropDelay <= m_mdGlobal->neighborPropDelayThresh) &&
                (m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity != m_timeAwareSystem->thisClock) && m_neighborRateRatioValid)
                    m_portGlobal->asCapable = true;
                else
                    m_portGlobal->asCapable = false;
                m_state = STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER:
            if(m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval)
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
    if (m_lostResponses <= m_mdGlobal->allowedLostResponses)
    m_lostResponses += 1;
    else
    {
        m_mdGlobal->isMeasuringDelay = false;
        m_portGlobal->asCapable = false;
    }
}

void MDPdelayReq::ExecuteSendPDelayReqState()
{
    m_pdelayReqSequenceId += 1;
    delete m_txPdelayReqPtr;
    m_txPdelayReqPtr = SetPdelayReq();
    TxPdelayReq(m_txPdelayReqPtr);
    m_pdelayIntervalTimer = m_timeAwareSystem->GetCurrentTime();
}

