#include "mdpdelayresp.h"

MDPdelayResp::MDPdelayResp(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdPdelayReq = false;
    m_rcvdMDTimestampReceive = false;
    m_txPdelayRespPtr = std::unique_ptr<PtpMessagePDelayResp>(new PtpMessagePDelayResp());
    m_txPdelayRespFollowUpPtr = std::unique_ptr<PtpMessagePDelayRespFollowUp>(new PtpMessagePDelayRespFollowUp());
    m_rcvdPdelayReqPtr = std::unique_ptr<PtpMessagePDelayReq>(new PtpMessagePDelayReq());
}

MDPdelayResp::~MDPdelayResp()
{
}

void MDPdelayResp::SetPdelayResp()
{
    PortIdentity identity;
    Timestamp receiveTime;

    identity.portNumber = m_portGlobal->thisPort;
    memset(identity.clockIdentity, 0, sizeof(identity.clockIdentity));

    m_txPdelayRespPtr->SetSourcePortIdentity(&identity);
    m_txPdelayRespPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    receiveTime.sec = m_rcvdPdelayReqPtr->GetReceiveTime().ns / NS_PER_SEC;
    receiveTime.ns = m_rcvdPdelayReqPtr->GetReceiveTime().ns % NS_PER_SEC;
    m_txPdelayRespPtr->SetRequestReceiptTimestamp(receiveTime);
    m_txPdelayRespPtr->SetCorrectionField(m_rcvdPdelayReqPtr->GetReceiveTime().ns_frac);
    m_txPdelayRespPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());

    /* sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity (see 8.5.2) */
    /* sequenceId is set equal to the sequenceId field of the corresponding Pdelay_Req message */
    /* requestReceiptTimestamp is set equal to the <pdelayReqEventIngressTimestamp> (see 11.3.2) of
     * the corresponding Pdelay_Req message, with any fractional ns portion truncated */
    /* correctionField is set equal to the fractional ns portion of the <pdelayReqEventIngressTimestamp> of the
     * corresponding Pdelay_Req message */
    /* requestingPortIdentity is set equal to the sourcePortIdentity field of the corresponding Pdelay_Req message */
    /* remaining parameters are set as specified in 11.4.2 and 11.4.6. */
}

void MDPdelayResp::TxPdelayResp()
{
    m_txPdelayRespPtr->SetSendTime(m_networkPort->SendEventMessage(m_txPdelayRespPtr.get()));
    printf("TxPdelayResp: %lu\n", m_txPdelayRespPtr->GetSendTime().ns);

    if(m_txPdelayRespPtr->GetSendTime().ns > 0)
        m_rcvdMDTimestampReceive = true;
}

void MDPdelayResp::SetPdelayRespFollowUp()
{
    PortIdentity identity;
    Timestamp sendTime;

    identity.portNumber = m_portGlobal->thisPort;
    memset(identity.clockIdentity, 0, sizeof(identity.clockIdentity));

    m_txPdelayRespFollowUpPtr->SetSourcePortIdentity(&identity);
    m_txPdelayRespFollowUpPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    sendTime.sec = m_txPdelayRespPtr->GetSendTime().ns / NS_PER_SEC;
    sendTime.ns = m_txPdelayRespPtr->GetSendTime().ns % NS_PER_SEC;
    m_txPdelayRespFollowUpPtr->SetRequestReceiptTimestamp(sendTime);
    m_txPdelayRespFollowUpPtr->SetCorrectionField(m_txPdelayRespPtr->GetSendTime().ns_frac);
    m_txPdelayRespFollowUpPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());

    /* a) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity (see 8.5.2),
     * b) sequenceId is set equal to the sequenceId field of the corresponding Pdelay_Req message,
     * c) responseOriginTimestamp is set equal to the <pdelayRespEventEgressTimestamp> (see 11.3.2) of
     * the corresponding Pdelay_Resp message, with any fractional ns truncated,
     * d) correctionField is set equal to the fractional ns portion of the <pdelayRespEventEgressTimestamp>
     * of the corresponding Pdelay_Resp message,
     * e) requestingPortIdentity is set equal to the sourcePortIdentity field of the corresponding Pdelay_Req
     * message, and
     * f) remaining parameters are set as specified in 11.4.2 and 11.4.6. */
}

void MDPdelayResp::TxPdelayRespFollowUp()
{
    m_networkPort->SendGenericMessage(m_txPdelayRespFollowUpPtr.get());
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
                SetPdelayResp();
                TxPdelayResp();

                m_state = STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;
            }
            break;

        case STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                SetPdelayRespFollowUp();
                TxPdelayRespFollowUp();
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
    m_rcvdPdelayReqPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayReqPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayReq = true;
}
