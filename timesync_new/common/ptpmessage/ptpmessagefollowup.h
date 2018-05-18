#ifndef PTPMESSAGEFOLLOWUP_H
#define PTPMESSAGEFOLLOWUP_H

#include "ptpmessagebase.h"

class PtpMessageFollowUp : public PtpMessageBase
{
public:

    struct FollowUpTLV
    {
        TlvType tlvType;
        uint16_t lengthField;
        unsigned char organizationId[3];
        uint32_t organizationSubType;
        /**
         * @brief The value of cumulativeScaledRateOffset is equal to (rateRatio – 1.0) × (2^41), truncated to the next smaller signed integer,
         * where rateRatio is the ratio of the frequency of the grandMaster to the frequency of the
         * LocalClock entity in the time-aware system that sends the message.
         */
        int32_t cumulativeScaledRateOffset;
        uint16_t gmTimeBaseIndicator;
        ScaledNs lastGmPhaseChange;
        /**
         * @brief The value of scaledLastGmFreqChange is the fractional frequency offset of the current grandmaster relative to the previous grandmaster,
         * at the time that the current grandmaster became grandmaster, or relative to itself prior to the last change in gmTimeBaseIndicator, multiplied
         * by 2^41 and truncated to the next smaller signed integer. The value is obtained by multiplying the lastGmFreqChange member of MDSyncSend
         * whose receipt causes the MD entity to send the Follow_Up message by 2^41 , and truncating to the next smaller signed integer.
         */
        int32_t scaledLastGmFreqChange;
    };

    PtpMessageFollowUp();


    virtual ~PtpMessageFollowUp();


    void GetPtpMessage(uint8_t* bytes);


    void ParsePackage(const uint8_t* bytes);


    Timestamp GetPreciseOriginTimestamp();


    void SetPreciseOriginTimestamp(Timestamp value);


    void GetFollowUpTLV(FollowUpTLV* tlv);


    void SetCumulativeScaledRateOffset(int32_t cumulativeScaledRateOffset);


    void SetGmTimeBaseIndicator(uint16_t gmTimeBaseIndicator);


    void SetLastGmPhaseChange(ScaledNs lastGmPhaseChange);


    void SetScaledLastGmFreqChange(int32_t scaledLastGmFreqChange);


private:


    Timestamp m_preciseOriginTimestamp;


    FollowUpTLV m_followUpTLV;
};

#endif // PTPMESSAGEFOLLOWUP_H
