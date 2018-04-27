#include "ptpmessagesignaling.h"

PtpMessageSignaling::PtpMessageSignaling()
{
    m_messageType = PTP_MESSSAGE_TYPE_SIGNALING;
    m_messageLength = 60;

    memset(m_targetPortIdentity.clockIdentity, 0x0, sizeof(m_targetPortIdentity.clockIdentity));
    m_targetPortIdentity.portNumber = 0x00FF;

    m_tlv.tlvType = TLV_TYPE_ORGANIZATION_EXTENSION;
    m_tlv.lengthField = 12;
    m_tlv.organizationId[0] = 0x00;
    m_tlv.organizationId[1] = 0x80;
    m_tlv.organizationId[2] = 0xC2;
    m_tlv.organizationSubType = 2;
    m_tlv.linkDelayInterval = 0;
    m_tlv.timeSyncInterval = 0;
    m_tlv.announceInterval = 0;
    m_tlv.flags = 0;

    m_control = 5;
}

PtpMessageSignaling::~PtpMessageSignaling()
{

}

void PtpMessageSignaling::GetPtpMessage(uint8_t *bytes)
{
    GetHeader(bytes);

    for(int i = 0; i < 8; i++)
        bytes[kMessageHeaderLength + i] = m_targetPortIdentity.clockIdentity[i];
    bytes[kMessageHeaderLength + 8] = m_targetPortIdentity.portNumber >> 8;
    bytes[kMessageHeaderLength + 9] = (uint8_t)(m_targetPortIdentity.portNumber);

    bytes[kMessageHeaderLength + 10] = m_tlv.tlvType >> 8;
    bytes[kMessageHeaderLength + 11] = (uint8_t)(m_tlv.tlvType);
    bytes[kMessageHeaderLength + 12] = m_tlv.lengthField >> 8;
    bytes[kMessageHeaderLength + 13] = (uint8_t)(m_tlv.lengthField);

    bytes[kMessageHeaderLength + 14] = m_tlv.organizationId[0];
    bytes[kMessageHeaderLength + 15] = m_tlv.organizationId[1];
    bytes[kMessageHeaderLength + 16] = m_tlv.organizationId[2];
    bytes[kMessageHeaderLength + 17] = m_tlv.organizationSubType >> 16;
    bytes[kMessageHeaderLength + 18] = m_tlv.organizationSubType >> 8;
    bytes[kMessageHeaderLength + 19] = m_tlv.organizationSubType;

    bytes[kMessageHeaderLength + 20] = m_tlv.linkDelayInterval;
    bytes[kMessageHeaderLength + 21] = m_tlv.timeSyncInterval;
    bytes[kMessageHeaderLength + 22] = m_tlv.announceInterval;
    bytes[kMessageHeaderLength + 23] = m_tlv.flags;

    bytes[kMessageHeaderLength + 24] = 0;
    bytes[kMessageHeaderLength + 25] = 0;
}

void PtpMessageSignaling::ParsePackage(const uint8_t* bytes)
{
    ParseHeader(bytes);

    for(int i = 0; i < 8; i++)
        m_targetPortIdentity.clockIdentity[i] = bytes[kMessageHeaderLength + i];
    m_targetPortIdentity.portNumber = (bytes[kMessageHeaderLength + 8] << 8) + bytes[kMessageHeaderLength + 9];

    m_tlv.tlvType = (TlvType)((bytes[kMessageHeaderLength + 10] << 8) + bytes[kMessageHeaderLength + 11]);
    m_tlv.lengthField = (bytes[kMessageHeaderLength + 12] << 8) + bytes[kMessageHeaderLength + 13];

    m_tlv.organizationId[0] = bytes[kMessageHeaderLength + 14];
    m_tlv.organizationId[1] = bytes[kMessageHeaderLength + 15];
    m_tlv.organizationId[2] = bytes[kMessageHeaderLength + 16];
    m_tlv.organizationSubType = (bytes[kMessageHeaderLength + 17] << 16) + (bytes[kMessageHeaderLength + 18] << 8) + bytes[kMessageHeaderLength + 19];

    m_tlv.linkDelayInterval = bytes[kMessageHeaderLength + 20];
    m_tlv.timeSyncInterval = bytes[kMessageHeaderLength + 21];
    m_tlv.announceInterval = bytes[kMessageHeaderLength + 22];
    m_tlv.flags = bytes[kMessageHeaderLength + 23];
}

int8_t PtpMessageSignaling::GetLinkDelayInterval()
{
    return m_tlv.linkDelayInterval;
}

void PtpMessageSignaling::SetLinkDelayInterval(int8_t interval)
{
    m_tlv.linkDelayInterval = interval;
}

int8_t PtpMessageSignaling::GetTimeSyncInterval()
{
    return m_tlv.timeSyncInterval;
}

void PtpMessageSignaling::SetTimeSyncInterval(int8_t interval)
{
    m_tlv.timeSyncInterval = interval;
}

int8_t PtpMessageSignaling::GetAnnounceInterval()
{
    return m_tlv.announceInterval;
}

void PtpMessageSignaling::SetAnnounceInterval(int8_t interval)
{
    m_tlv.announceInterval = interval;
}

bool PtpMessageSignaling::GetComputeNeighborRateRatio()
{
    return (m_tlv.flags & (1 << FLAG_COMPUTE_NEIGHBOR_RATE_RATIO)) == 1 << FLAG_COMPUTE_NEIGHBOR_RATE_RATIO;
}

void PtpMessageSignaling::SetComputeNeighborRateRatio(bool compute)
{
    m_tlv.flags = compute ? (m_tlv.flags | (1 << FLAG_COMPUTE_NEIGHBOR_RATE_RATIO)) : (m_tlv.flags & ~(1 << FLAG_COMPUTE_NEIGHBOR_RATE_RATIO));
}

bool PtpMessageSignaling::GetComputeNeighborPropDelay()
{
    return (m_tlv.flags & (1 << FLAG_COMPUTE_NEIGHBOR_PROP_DELAY)) == 1 << FLAG_COMPUTE_NEIGHBOR_PROP_DELAY;
}

void PtpMessageSignaling::SetComputeNeighborPropDelay(bool compute)
{
    m_tlv.flags = compute ? (m_tlv.flags | (1 << FLAG_COMPUTE_NEIGHBOR_PROP_DELAY)) : (m_tlv.flags & ~(1 << FLAG_COMPUTE_NEIGHBOR_PROP_DELAY));
}
