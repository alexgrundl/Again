#include "ptpmessagefollowup.h"

PtpMessageFollowUp::PtpMessageFollowUp()
{
    m_messageLength = 76;
    m_messageType = PTP_MESSSAGE_TYPE_SYNC;
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
//    for(int i = 0; i < 20; i++)
//        bytes[kMessageHeaderLength + i] = 0;
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
