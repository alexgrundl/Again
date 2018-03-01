#include "ptpmessagebase.h"

const uint8_t PtpMessageBase::kMacMulticast[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};

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

void PtpMessageBase::ParseHeader(const uint8_t* bytes)
{
    m_messageType = (PtpMessageType)(bytes[0] & 0x0F);
    m_transportSpecific = (bytes[0] & 0xF0);
    m_versionPTP = (bytes[1] & 0x0F);
    m_messageLength = (bytes[2] << 8) + bytes[3];
    m_domainNumber = bytes[4];
    m_flags = (bytes[6] << 8) + bytes[7];
    m_correctionField = 0;
    for(int i = 0; i < 8; i++)
        m_correctionField += ((uint64_t)bytes[8 + i] << (56 - i * 8));
    for(int i = 0; i < 8; i++)
        m_sourcePortIdentity.clockIdentity[i] = bytes[20 + i];
    m_sourcePortIdentity.portNumber = (bytes[28] << 8) + bytes[29];
    m_sequenceId = (bytes[30] << 8) + bytes[31];
    m_control = bytes[32];
    m_logMessageInterval = bytes[33];
}

PtpMessageType PtpMessageBase::ParseMessageType(const uint8_t* bytes)
{
    return (PtpMessageType)(bytes[0] & 0x0F);
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

void PtpMessageBase::GetClockIdentity(const uint8_t* portMac, uint8_t* clockIdentity)
{
    clockIdentity[0] = portMac[0];
    clockIdentity[1] = portMac[1];
    clockIdentity[2] = portMac[2];

    clockIdentity[3] = 0xFF;
    clockIdentity[4] = 0xFE;

    clockIdentity[5] = portMac[3];
    clockIdentity[6] = portMac[4];
    clockIdentity[7] = portMac[5];
}

bool PtpMessageBase::IsPortIdentityEqual(PortIdentity* identity1, PortIdentity* identity2)
{
    return memcmp(identity1->clockIdentity, identity2->clockIdentity, sizeof(identity2->clockIdentity)) == 0 &&
            identity1->portNumber == identity2->portNumber;
}
