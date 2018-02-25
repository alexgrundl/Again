#include "ptpmessagefollowup.h"

PtpMessageFollowUp::PtpMessageFollowUp()
{
    m_messageLength = 76;
    m_messageType = PTP_MESSSAGE_TYPE_FOLLOW_UP;
    m_control = 2;

    m_preciseOriginTimestamp.sec = 0;
    m_preciseOriginTimestamp.ns = 0;

    m_followUpTLV.tlvType = TLV_TYPE_ORGANIZATION_EXTENSION;
    m_followUpTLV.lengthField = 28;
    m_followUpTLV.organizationId[0] = 0x00;
    m_followUpTLV.organizationId[1] = 0x80;
    m_followUpTLV.organizationId[2] = 0xC2;
    m_followUpTLV.organizationSubType = 1;
}

PtpMessageFollowUp::~PtpMessageFollowUp()
{

}

void PtpMessageFollowUp::GetMessage(uint8_t *bytes)
{
    GetHeader(bytes);

    for(int i = 0; i < 6; i++)
        bytes[kMessageHeaderLength + i] = (uint8_t)(m_preciseOriginTimestamp.sec >> (40 - i * 8));
    for(int i = 0; i < 4; i++)
        bytes[kMessageHeaderLength + 6 + i] = (uint8_t)(m_preciseOriginTimestamp.ns >> (24 - i * 8));

    bytes[kMessageHeaderLength + 10] = m_followUpTLV.tlvType >> 8;
    bytes[kMessageHeaderLength + 11] = m_followUpTLV.tlvType;
    bytes[kMessageHeaderLength + 12] = m_followUpTLV.lengthField >> 8;
    bytes[kMessageHeaderLength + 13] = m_followUpTLV.lengthField;

    bytes[kMessageHeaderLength + 14] = m_followUpTLV.organizationId[0];
    bytes[kMessageHeaderLength + 15] = m_followUpTLV.organizationId[1];
    bytes[kMessageHeaderLength + 16] = m_followUpTLV.organizationId[2];

    bytes[kMessageHeaderLength + 17] = m_followUpTLV.organizationSubType >> 16;
    bytes[kMessageHeaderLength + 18] = m_followUpTLV.organizationSubType >> 8;
    bytes[kMessageHeaderLength + 19] = m_followUpTLV.organizationSubType;

    for(int i = 0; i < 4; i++)
        bytes[kMessageHeaderLength + 20 + i] = (uint8_t)(m_followUpTLV.cumulativeScaledRateOffset >> (24 - i * 8));

    bytes[kMessageHeaderLength + 24] = m_followUpTLV.gmTimeBaseIndicator >> 8;
    bytes[kMessageHeaderLength + 25] = m_followUpTLV.gmTimeBaseIndicator;

    bytes[kMessageHeaderLength + 26] = 0;
    bytes[kMessageHeaderLength + 27] = 0;
    for(int i = 0; i < 8; i++)
        bytes[kMessageHeaderLength + 28 + i] = (uint8_t)(m_followUpTLV.lastGmPhaseChange.ns >> (56 - i * 8));

    bytes[kMessageHeaderLength + 36] = m_followUpTLV.lastGmPhaseChange.ns_frac >> 8;
    bytes[kMessageHeaderLength + 37] = m_followUpTLV.lastGmPhaseChange.ns_frac;

    for(int i = 0; i < 4; i++)
        bytes[kMessageHeaderLength + 38 + i] = (uint8_t)(m_followUpTLV.scaledLastGmFreqChange >> (24 - i * 8));
}

Timestamp PtpMessageFollowUp::GetPreciseOriginTimestamp()
{
    return m_preciseOriginTimestamp;
}

void PtpMessageFollowUp::SetPreciseOriginTimestamp(Timestamp value)
{
    m_preciseOriginTimestamp.sec = value.sec;
    m_preciseOriginTimestamp.ns = value.ns;
}

void PtpMessageFollowUp::GetFollowUpTLV(FollowUpTLV* tlv)
{
    tlv->tlvType = m_followUpTLV.tlvType;
    tlv->lengthField  = m_followUpTLV.lengthField;
    memcpy(tlv->organizationId, m_followUpTLV.organizationId, sizeof(m_followUpTLV.organizationId));
    tlv->organizationSubType = m_followUpTLV.organizationSubType;
    tlv->cumulativeScaledRateOffset = m_followUpTLV.cumulativeScaledRateOffset;
    tlv->gmTimeBaseIndicator = m_followUpTLV.gmTimeBaseIndicator;
    tlv->lastGmPhaseChange = m_followUpTLV.lastGmPhaseChange;
    tlv->scaledLastGmFreqChange = m_followUpTLV.scaledLastGmFreqChange;
}

void PtpMessageFollowUp::SetCumulativeScaledRateOffset(int32_t cumulativeScaledRateOffset)
{
    m_followUpTLV.cumulativeScaledRateOffset = cumulativeScaledRateOffset;
}

void PtpMessageFollowUp::SetGmTimeBaseIndicator(uint16_t gmTimeBaseIndicator)
{
    m_followUpTLV.gmTimeBaseIndicator = gmTimeBaseIndicator;
}

void PtpMessageFollowUp::SetLastGmPhaseChange(ScaledNs lastGmPhaseChange)
{
    m_followUpTLV.lastGmPhaseChange = lastGmPhaseChange;
}

void PtpMessageFollowUp::SetScaledLastGmFreqChange(int32_t scaledLastGmFreqChange)
{
    m_followUpTLV.scaledLastGmFreqChange = scaledLastGmFreqChange;
}
