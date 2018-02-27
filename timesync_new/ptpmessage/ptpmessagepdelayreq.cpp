#include "ptpmessagepdelayreq.h"

PtpMessagePDelayReq::PtpMessagePDelayReq()
{
    m_messageLength = 54;
    m_messageType = PTP_MESSSAGE_TYPE_PDELAY_REQ;
    m_control = 5;
}

void PtpMessagePDelayReq::GetMessage(uint8_t* bytes)
{
    GetHeader(bytes);
    for(int i = 0; i < 20; i++)
        bytes[kMessageHeaderLength + i] = 0;
}

void PtpMessagePDelayReq::ParsePackage(const uint8_t* bytes)
{
    ParseHeader(bytes);
}
