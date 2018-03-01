#ifndef PORTANNOUNCEINFORMATION_H
#define PORTANNOUNCEINFORMATION_H

#include "statemachinebase.h"
#include "ptpmessage/ptpmessageannounce.h"

class PortAnnounceInformation : public StateMachineBasePort
{
public:

    PortAnnounceInformation(TimeAwareSystem* timeAwareSystem, PortGlobal* port);


    ~PortAnnounceInformation();


    void SetAnnounce(PtpMessageAnnounce* message);


    void ProcessState();


private:

    enum MasterInfo
    {
        MASTER_INFO_SUPERIOR,
        MASTER_INFO_REPEATED,
        MASTER_INFO_INFERIOR,
        MASTER_INFO_OTHER
    };

    /**
     * @brief A variable used to save the time at which announce receipt timeout occurs.
     */
    UScaledNs m_announceReceiptTimeoutTime;

    /**
     * @brief The messagePriorityVector corresponding to the received Announce information.
     */
    PriorityVector m_messagePriority;

    /**
     * @brief An Enumeration2 that holds the value returned by rcvInfo() (see 10.3.11.2.1).
     */
    MasterInfo m_rcvdInfo;


    /**
     * @brief Decodes the messagePriorityVector (see 10.3.4 and 10.3.5) and stepsRemoved 10.5.3.2.6) field from the Announce information pointed
     * to by rcvdAnnouncePtr (see 10.3.9.11)
     * @param rcvdAnnouncePtr The announce message to decode.
     * @return
     */
    MasterInfo RcvInfo(PtpMessageAnnounce* rcvdAnnouncePtr);

    /**
     * @brief saves the flags leap61, leap59, currentUtcOffsetValid, timeTraceable, and frequencyTraceable, and the fields currentUtcOffset and timeSource,
     * of the received Announce message for this port. The values are saved in the per-port global variables annLeap61, annLeap59, annCurrentUTCOffsetValid,
     * annTimeTraceable, annFrequencyTraceable, annCurrentUTCOffset, and annTimeSource (see 10.3.9.14 through 10.3.9.20).
     * @param rcvdAnnouncePtr The announce message to decode.
     */
    void RecordOtherAnnounceInfo(PtpMessageAnnounce* rcvdAnnouncePtr);


    void ExecuteDisabledState();


    void ExecuteAgedState();


    void ExecuteUpdateState();

};

#endif // PORTANNOUNCEINFORMATION_H
