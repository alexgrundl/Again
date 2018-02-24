#include "mdpdelayresp.h"

MDPdelayResp::MDPdelayResp(TimeAwareSystem *timeAwareSystem, PortGlobal *port, MDGlobal *mdGlobal) :
    StateMachineBaseMD(timeAwareSystem, port, mdGlobal)
{
    m_rcvdPdelayReq = false;
    m_rcvdMDTimestampReceive = false;
    m_txPdelayRespPtr = NULL;
    m_txPdelayRespFollowUpPtr = NULL;
}

MDPdelayResp::~MDPdelayResp()
{
    delete m_txPdelayRespPtr;
    delete m_txPdelayRespFollowUpPtr;
}

PtpMessagePDelayResp* MDPdelayResp::SetPdelayResp()
{
    PtpMessagePDelayResp* pdelayRespPtr = new PtpMessagePDelayResp();

    /* sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity (see 8.5.2) */
    /* sequenceId is set equal to the sequenceId field of the corresponding Pdelay_Req message */
    /* requestReceiptTimestamp is set equal to the <pdelayReqEventIngressTimestamp> (see 11.3.2) of
     * the corresponding Pdelay_Req message, with any fractional ns portion truncated */
    /* correctionField is set equal to the fractional ns portion of the <pdelayReqEventIngressTimestamp> of the
     * corresponding Pdelay_Req message */
    /* requestingPortIdentity is set equal to the sourcePortIdentity field of the corresponding Pdelay_Req message */
    /* remaining parameters are set as specified in 11.4.2 and 11.4.6. */

    return pdelayRespPtr;
}

void MDPdelayResp::TxPdelayResp(PtpMessagePDelayResp* txPdelayRespPtr)
{

}

PtpMessagePDelayRespFollowUp* MDPdelayResp::SetPdelayRespFollowUp()
{
    PtpMessagePDelayRespFollowUp* pdelayRespFollowUpPtr = new PtpMessagePDelayRespFollowUp();

    /* a) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity (see 8.5.2),
     * b) sequenceId is set equal to the sequenceId field of the corresponding Pdelay_Req message,
     * c) responseOriginTimestamp is set equal to the <pdelayRespEventEgressTimestamp> (see 11.3.2) of
     * the corresponding Pdelay_Resp message, with any fractional ns truncated,
     * d) correctionField is set equal to the fractional ns portion of the <pdelayRespEventEgressTimestamp>
     * of the corresponding Pdelay_Resp message,
     * e) requestingPortIdentity is set equal to the sourcePortIdentity field of the corresponding Pdelay_Req
     * message, and
     * f) remaining parameters are set as specified in 11.4.2 and 11.4.6. */

    return pdelayRespFollowUpPtr;
}

void MDPdelayResp::TxPdelayRespFollowUp(PtpMessagePDelayRespFollowUp* txFollowUpPtr)
{

}

void MDPdelayResp::ProcessState()
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
                m_rcvdPdelayReq = false;
                m_state = STATE_INITIAL_WAITING_FOR_PDELAY_REQ;
            }
            break;

        case STATE_INITIAL_WAITING_FOR_PDELAY_REQ:
        case STATE_WAITING_FOR_PDELAY_REQ:
            if(m_rcvdPdelayReq)
            {
                m_rcvdPdelayReq = false;
                delete m_txPdelayRespPtr;
                m_txPdelayRespPtr = SetPdelayResp();
                TxPdelayResp(m_txPdelayRespPtr);

                m_state = STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;
            }
            break;

        case STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                delete m_txPdelayRespFollowUpPtr;
                m_txPdelayRespFollowUpPtr = SetPdelayRespFollowUp();
                TxPdelayRespFollowUp(m_txPdelayRespFollowUpPtr);
                m_state = STATE_WAITING_FOR_PDELAY_REQ;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
