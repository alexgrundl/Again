#ifndef PTPMESSAGESIGNALING_H
#define PTPMESSAGESIGNALING_H

#include "ptpmessagebase.h"

class PtpMessageSignaling : public PtpMessageBase
{
public:

    enum SignalingFlags
    {
        FLAG_COMPUTE_NEIGHBOR_RATE_RATIO = 1,
        FLAG_COMPUTE_NEIGHBOR_PROP_DELAY
    };

    struct SignalingTLV
    {
        TlvType tlvType;
        uint16_t lengthField;
        unsigned char organizationId[3];
        uint32_t organizationSubType;

        /**
         * @brief The value is the logarithm to base 2 of the mean time interval, desired by the port that sends this TLV, between successive
         * Pdelay_Req messages sent by the port at the other end of the link. The format and allowed values of linkDelayInterval are the same
         * as the format and allowed values of initialLogPdelayReqInterval.
         */
        int8_t linkDelayInterval;

        /**
         * @brief The value is the logarithm to base 2 of the mean time interval, desired by the port that sends this TLV, between successive
         * time-synchronization event messages sent by the port at the other end of the link. The format and allowed values of timeSyncInterval
         * are the same as the format and allowed values of initialLogSyncInterval.
         * When a signaling message that contains this TLV is sent by a port, the value of syncReceiptTimeoutTimeInterval for that port shall
         * be set equal to syncReceiptTimeout multiplied by the value of the interval, in seconds, reflected by timeSyncInterval.
         */
        int8_t timeSyncInterval;

        /**
         * @brief The value is the logarithm to base 2 of the mean time interval, desired by the port that sends this TLV, between successive
         * Announce messages sent by the port at the other end of the link. The format and allowed values of announceInterval are the same as
         * the format and allowed values of initialLogAnnounceInterval.
         * When a signaling message that contains this TLV is sent by a port, the value of announceReceiptTimeoutTimeInterval for that port
         * shall be set equal to announceReceiptTimeout multiplied by the value of the interval, in seconds, reflected by announceInterval.
         */
        int8_t announceInterval;

        /**
         * @brief Bit 1: computeNeighborRateRatio, bit 2: computeNeighborPropDelay, other bits have to be 0
         */
        uint8_t flags;
    };


    PtpMessageSignaling();


    virtual ~PtpMessageSignaling();


    void ParsePackage(const uint8_t* bytes);


    void GetPtpMessage(uint8_t* bytes);



    int8_t GetLinkDelayInterval();


    void SetLinkDelayInterval(int8_t interval);


    int8_t GetTimeSyncInterval();


    void SetTimeSyncInterval(int8_t interval);


    int8_t GetAnnounceInterval();


    void SetAnnounceInterval(int8_t interval);


    bool GetComputeNeighborRateRatio();


    void SetComputeNeighborRateRatio(bool compute);


    bool GetComputeNeighborPropDelay();


    void SetComputeNeighborPropDelay(bool compute);

private:

    PortIdentity m_targetPortIdentity;

    SignalingTLV m_tlv;
};

#endif // PTPMESSAGESIGNALING_H
