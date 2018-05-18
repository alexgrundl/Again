#ifndef PTPMESSAGEANNOUNCE_H
#define PTPMESSAGEANNOUNCE_H

#include <vector>

#include "ptpmessagebase.h"

class PtpMessageAnnounce : public PtpMessageBase
{
public:

    struct AnnounceTLV
    {
        TlvType tlvType;
        uint16_t lengthField;
        /**
         * @brief The value of pathSequence is a ClockIdentity array. The array elements are the clockIdentities of the successive time-aware systems that
         * receive and send an Announce message. The quantity N is the number of time-aware systems, including the grandmaster, that the Announce
         * information has traversed.
         * NOTE—N is equal to stepsRemoved+1 (see 10.5.3.2.6). The size of the pathSequence array increases by 1 for each time-aware system that the
         * Announce information traverses.
         */
        std::vector<uint8_t*> pathSequence;
    };

    PtpMessageAnnounce();


    ~PtpMessageAnnounce();


    void ParsePackage(const uint8_t* bytes);


    void GetPtpMessage(uint8_t* bytes);


    int16_t GetCurrentUtcOffset();


    void SetCurrentUtcOffset(int16_t offset);


    uint8_t GetGrandmasterPriority1();


    void SetGrandmasterPriority1(uint8_t priority);


    ClockQuality GetGrandmasterClockQuality();


    void SetGrandmasterClockQuality(ClockQuality quality);


    uint8_t GetGrandmasterPriority2();


    void SetGrandmasterPriority2(uint8_t priority);


    void GetGrandmasterIdentity(uint8_t* identity);


    void SetGrandmasterIdentity(uint8_t* identity);


    uint16_t GetStepsRemoved();


    void SetStepsRemoved(uint16_t steps);


    ClockTimeSource GetTimeSource();


    void SetTimeSource(ClockTimeSource timeSource);


    AnnounceTLV GetTLV();


    std::vector<uint8_t *> GetPathSequence();


    void SetPathSequence(std::vector<uint8_t*> pathSequence);


    bool GetFlagLeap61();


    void SetFlagLeap61(bool enable);


    bool GetFlagLeap59();


    void SetFlagLeap59(bool enable);


    bool GetCurrentUtcOffsetValid();


    void SetCurrentUtcOffsetValid(bool enable);


    bool GetTimeTraceable();


    void SetTimeTraceable(bool enable);


    bool GetFrequencyTraceable();


    void SetFrequencyTraceable(bool enable);


private:

    /**
     * @brief The value is the value of currentUtcOffset (see 10.3.8.9) for the time-aware system that transmits the Announce message.
     */
    int16_t m_currentUtcOffset;
    /**
     * @brief The value is the value of the priority1 component of the rootSystemIdentity of the gmPriorityVector (see 10.3.5) of the time-aware
     * system that transmits the Announce message.
     */
    uint8_t m_grandmasterPriority1;
    /**
     * @brief The value is the clockQuality formed by the clockClass, clockAccuracy, and offsetScaledLogVariance of the rootSystemIdentity of the
     * gmPriorityVector (see 10.3.5) of the time-aware system that transmits the Announce message.
     */
    ClockQuality m_grandmasterClockQuality;

    /**
     * @brief The value is the value of the priority2 component of the rootSystemIdentity of the gmPriorityVector (see 10.3.5) of the time-aware
     * system that transmits the Announce message.
     */
    uint8_t m_grandmasterPriority2;

    /**
     * @brief The value is the value of the clockIdentity component of the rootSystemIdentity of the gmPriorityVector (see 10.3.5) of the time-aware
     * system that transmits the Announce message.
     */
    uint8_t m_grandmasterIdentity[CLOCK_ID_LENGTH];

    /**
     * @brief The value is the value of masterStepsRemoved (see 10.3.8.3) for the time-aware system that transmits the Announce message.
     */
    uint16_t m_stepsRemoved;

    /**
     * @brief The value is the value of timeSource (see 10.3.8.10) for the time-aware system that transmits the Announce message.
     */
    ClockTimeSource m_timeSource;


    AnnounceTLV m_tlv;
};

#endif // PTPMESSAGEANNOUNCE_H
