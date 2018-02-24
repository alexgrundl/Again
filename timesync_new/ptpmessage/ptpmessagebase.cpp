#include "ptpmessagebase.h"

PtpMessageBase::PtpMessageBase()
{
    m_versionPTP = 2;
    m_transportSpecific = 1;
    m_messageType = PTP_MESSSAGE_TYPE_SYNC;
    m_messageLength = 0;
    m_domainNumber = 0;
    m_flags = (1 << FLAG_TIMESCALE);
    m_correctionField = 0;
    m_sourcePortIdentity.portNumber = 0;
    memset(m_sourcePortIdentity.clockIdentity, 0, sizeof(m_sourcePortIdentity.clockIdentity));
    m_sequenceId = 0;
    m_control = 0;
    m_logMessageInterval = 0;
}

PtpMessageBase::~PtpMessageBase()
{

}

void PtpMessageBase::GetHeader(uint8_t* bytes)
{
    bytes[0] = (uint8_t)m_messageType + (1 << 4) * m_transportSpecific;
    bytes[1] = m_versionPTP;
    bytes[2] = m_messageLength >> 8;
    bytes[3] = (uint8_t)m_messageLength;
    bytes[4] = m_domainNumber;
    bytes[5] = 0;
    bytes[6] = m_flags >> 8;
    bytes[7] = (uint8_t)m_flags;
    for(int i = 0; i < 8; i++)
        bytes[8 + i] = (uint8_t)(m_correctionField >> (56 - i * 8));
    for(int i = 0; i < 4; i++)
        bytes[16 + i] = 0;
    for(int i = 0; i < 8; i++)
        bytes[20 + i] = m_sourcePortIdentity.clockIdentity[i];
    bytes[28] = m_sourcePortIdentity.portNumber >> 8;
    bytes[29] = (uint8_t)(m_sourcePortIdentity.portNumber);
    bytes[30] = m_sequenceId >> 8;
    bytes[31] = (uint8_t)m_sequenceId;
    bytes[32] = m_control;
    bytes[33] = m_logMessageInterval;
}

uint8_t PtpMessageBase::GetVersionPTP()
{
    return m_versionPTP;
}

uint8_t PtpMessageBase::GetTransportSpecific()
{
    return m_transportSpecific;
}

PtpMessageType PtpMessageBase::GetMessageType()
{
    return m_messageType;
}

uint16_t PtpMessageBase::GetMessageLength()
{
    return m_messageLength;
}

uint8_t PtpMessageBase::GetDomainNumber()
{
    return m_domainNumber;
}

void PtpMessageBase::SetDomainNumber(uint8_t value)
{
    m_domainNumber = value;
}

uint16_t PtpMessageBase::GetFlags()
{
    return m_flags;
}


int64_t PtpMessageBase::GetCorrectionField()
{
    return m_correctionField;
}

void PtpMessageBase::SetCorrectionField(int64_t value)
{
    m_correctionField = value;
}

PortIdentity PtpMessageBase::GetSourcePortIdentity()
{
//    memcpy(value->clockIdentity, m_sourcePortIdentity.clockIdentity, sizeof(m_sourcePortIdentity.clockIdentity));
//    value->portNumber = m_sourcePortIdentity.portNumber;

    return m_sourcePortIdentity;
}

void PtpMessageBase::SetSourcePortIdentity(PortIdentity* value)
{
    memcpy(m_sourcePortIdentity.clockIdentity, value->clockIdentity, sizeof(value->clockIdentity));
    m_sourcePortIdentity.portNumber = value->portNumber;
}

uint16_t PtpMessageBase::GetSequenceID()
{
    return m_sequenceId;
}

void PtpMessageBase::SetSequenceID(uint16_t value)
{
    m_sequenceId = value;
}

uint8_t PtpMessageBase::GetControl()
{
    return m_control;
}

int8_t PtpMessageBase::GetLogMessageInterval()
{
    return m_logMessageInterval;
}

void PtpMessageBase::SetLogMessageInterval(int8_t value)
{
    m_logMessageInterval = value;
}

UScaledNs PtpMessageBase::GetReceiveTime()
{
    return m_receiveTime;
}

UScaledNs PtpMessageBase::GetSendTime()
{
    return m_sendTime;
}

void PtpMessageBase::SetReceiveTime(UScaledNs value)
{
    m_receiveTime = value;
}

void PtpMessageBase::SetSendTime(UScaledNs value)
{
    m_sendTime = value;
}
