#include "mdpdelayresp.h"

MDPdelayResp::MDPdelayResp(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdPdelayReq = false;
    m_rcvdMDTimestampReceive = false;
    m_txPdelayRespPtr = new PtpMessagePDelayResp();
    m_txPdelayRespFollowUpPtr = new PtpMessagePDelayRespFollowUp();
    m_rcvdPdelayReqPtr = new PtpMessagePDelayReq();
}

MDPdelayResp::~MDPdelayResp()
{
    delete m_txPdelayRespPtr;
    delete m_txPdelayRespFollowUpPtr;
    delete m_rcvdPdelayReqPtr;
}

void MDPdelayResp::SetPdelayResp()
{
    PortIdentity identity;
    Timestamp receiveTime;

    identity.portNumber = m_portGlobal->identity.portNumber;
    memcpy(identity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), sizeof(identity.clockIdentity));

    m_txPdelayRespPtr->SetSourcePortIdentity(&identity);
    m_txPdelayRespPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    receiveTime.sec = m_rcvdPdelayReqPtr->GetReceiveTime().ns / NS_PER_SEC;
    receiveTime.ns = m_rcvdPdelayReqPtr->GetReceiveTime().ns % NS_PER_SEC;
    m_txPdelayRespPtr->SetRequestReceiptTimestamp(receiveTime);
    m_txPdelayRespPtr->SetCorrectionField(m_rcvdPdelayReqPtr->GetReceiveTime().ns_frac);
    m_txPdelayRespPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());

    m_txPdelayRespPtr->SetDomainNumber(m_rcvdPdelayReqPtr->GetDomainNumber());
}

void MDPdelayResp::TxPdelayResp()
{
    m_txPdelayRespPtr->SetSendTime(m_networkPort->SendEventMessage(m_txPdelayRespPtr));
//    printf("TxPdelayResp: %lu\n", m_txPdelayRespPtr->GetSendTime().ns);

//    if(m_txPdelayRespPtr->GetSendTime().ns > 0)
        m_rcvdMDTimestampReceive = true;
}

void MDPdelayResp::SetPdelayRespFollowUp()
{
    PortIdentity identity;
    Timestamp sendTime;

    identity.portNumber = m_portGlobal->identity.portNumber;
    memcpy(identity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), sizeof(identity.clockIdentity));

    m_txPdelayRespFollowUpPtr->SetSourcePortIdentity(&identity);
    m_txPdelayRespFollowUpPtr->SetSequenceID(m_rcvdPdelayReqPtr->GetSequenceID());
    sendTime.sec = m_txPdelayRespPtr->GetSendTime().ns / NS_PER_SEC;
    sendTime.ns = m_txPdelayRespPtr->GetSendTime().ns % NS_PER_SEC;
    m_txPdelayRespFollowUpPtr->SetRequestReceiptTimestamp(sendTime);
    m_txPdelayRespFollowUpPtr->SetCorrectionField(m_txPdelayRespPtr->GetSendTime().ns_frac);
    m_txPdelayRespFollowUpPtr->SetRequestingPortIdentity(m_rcvdPdelayReqPtr->GetSourcePortIdentity());
}

void MDPdelayResp::TxPdelayRespFollowUp()
{
    m_networkPort->SendGenericMessage(m_txPdelayRespFollowUpPtr);
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
    m_portGlobal->pdelayCount++;
    m_rcvdPdelayReq = true;
}
