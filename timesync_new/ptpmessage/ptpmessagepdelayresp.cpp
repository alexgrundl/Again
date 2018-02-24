#include "ptpmessagepdelayresp.h"

PtpMessagePDelayResp::PtpMessagePDelayResp()
{
    m_messageLength = 54;
    m_messageType = PTP_MESSSAGE_TYPE_PDELAY_RESP;
    m_flags |= (1 << FLAG_TWO_STEP);
    m_control = 5;

    m_requestReceiptTimestamp.sec = 0;
    m_requestReceiptTimestamp.ns = 0;

    m_requestingPortIdentity.portNumber = 0;
    memset(m_requestingPortIdentity.clockIdentity, 0, sizeof(m_requestingPortIdentity.clockIdentity));
}

void PtpMessagePDelayResp::GetMessage(uint8_t* bytes)
{
    GetHeader(bytes);

    for(int i = 0; i < 6; i++)
        bytes[kMessageHeaderLength + i] = (uint8_t)(m_requestReceiptTimestamp.sec >> (40 - i * 8));
    for(int i = 0; i < 4; i++)
        bytes[kMessageHeaderLength + 6 + i] = (uint8_t)(m_requestReceiptTimestamp.ns >> (24 - i * 8));

    for(int i = 0; i < 8; i++)
        bytes[kMessageHeaderLength + 10 + i] = m_requestingPortIdentity.clockIdentity[i];
    bytes[kMessageHeaderLength + 18] = m_requestingPortIdentity.portNumber >> 8;
    bytes[kMessageHeaderLength + 19] = (uint8_t)(m_requestingPortIdentity.portNumber);
}

PortIdentity* PtpMessagePDelayResp::GetRequestingPortIdentity()
{
    return &m_requestingPortIdentity;
}

Timestamp PtpMessagePDelayResp::GetRequestReceiptTimestamp()
{
    return m_requestReceiptTimestamp;
}
