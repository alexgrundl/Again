#include "mdpdelayresp.h"

MDPdelayResp::MDPdelayResp(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdPdelayReq = false;
    m_rcvdMDTimestampReceive = false;
    m_txPdelayRespPtr = NULL;
    m_txPdelayRespFollowUpPtr = NULL;
    m_rcvdPdelayReqPtr = NULL;
}

MDPdelayResp::~MDPdelayResp()
{
    delete m_txPdelayRespPtr;
    delete m_txPdelayRespFollowUpPtr;
    delete m_rcvdPdelayReqPtr;
}

PtpMessagePDelayResp* MDPdelayResp::SetPdelayResp()
{
    PtpMessagePDelayResp* pdelayRespPtr = new PtpMessagePDelayResp();
    PortIdentity identity;
    Timestamp receiveTime;

    identity.portNumber = m_portGlobal->thisPort;
    memset(identity.clockIdentity, 0, sizeof(identity.clockIdentity));

    pdelayRespPtr->SetSourcePortIdentity(&identity);
    pdelayRespPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    receiveTime.sec = m_rcvdPdelayReqPtr->GetReceiveTime().ns / NS_PER_SEC;
    receiveTime.ns = m_rcvdPdelayReqPtr->GetReceiveTime().ns % NS_PER_SEC;
    pdelayRespPtr->SetRequestReceiptTimestamp(receiveTime);
    pdelayRespPtr->SetCorrectionField(m_rcvdPdelayReqPtr->GetReceiveTime().ns_frac);
    pdelayRespPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());

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
    txPdelayRespPtr->SetSendTime(m_networkPort->SendEventMessage(txPdelayRespPtr));
    printf("TxPdelayResp: %lu\n", txPdelayRespPtr->GetSendTime().ns);

    if(txPdelayRespPtr->GetSendTime().ns > 0)
        m_rcvdMDTimestampReceive = true;
}

PtpMessagePDelayRespFollowUp* MDPdelayResp::SetPdelayRespFollowUp()
{
    PtpMessagePDelayRespFollowUp* pdelayRespFollowUpPtr = new PtpMessagePDelayRespFollowUp();
    PortIdentity identity;
    Timestamp sendTime;

    identity.portNumber = m_portGlobal->thisPort;
    memset(identity.clockIdentity, 0, sizeof(identity.clockIdentity));

    pdelayRespFollowUpPtr->SetSourcePortIdentity(&identity);
    pdelayRespFollowUpPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    sendTime.sec = m_txPdelayRespPtr->GetSendTime().ns / NS_PER_SEC;
    sendTime.ns = m_txPdelayRespPtr->GetSendTime().ns % NS_PER_SEC;
    pdelayRespFollowUpPtr->SetRequestReceiptTimestamp(sendTime);
    pdelayRespFollowUpPtr->SetCorrectionField(m_txPdelayRespPtr->GetSendTime().ns_frac);
    pdelayRespFollowUpPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());

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
    m_networkPort->SendGenericMessage(txFollowUpPtr);
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

void MDPdelayResp::SetPDelayRequest(IReceivePackage* package)
{
    delete m_rcvdPdelayReqPtr;
    m_rcvdPdelayReqPtr = new PtpMessagePDelayReq();
    m_rcvdPdelayReqPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayReqPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayReq = true;
}
