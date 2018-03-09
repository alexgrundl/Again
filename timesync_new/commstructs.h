#ifndef COMM_STRUCTS
#define COMM_STRUCTS

#include <vector>
#include <time.h>

#include "types.h"
#include "ptpmessage/ptpmessageannounce.h"

struct MDSyncSend
{
    ScaledNs followUpCorrectionField;
    PortIdentity sourcePortIdentity;
    int8_t logMessageInterval;
    Timestamp preciseOriginTimestamp;
    UScaledNs upstreamTxTime;
    double rateRatio;
    uint16_t gmTimeBaseIndicator;
    ScaledNs lastGmPhaseChange;
    double lastGmFreqChange;
};

typedef MDSyncSend MDSyncReceive;

struct PortSyncSync
{
    uint16_t localPortNumber;
    UScaledNs syncReceiptTimeoutTime;
    ScaledNs followUpCorrectionField;
    PortIdentity sourcePortIdentity;
    int8_t logMessageInterval;
    Timestamp preciseOriginTimestamp;
    UScaledNs upstreamTxTime;
    double rateRatio;
    uint16_t gmTimeBaseIndicator;
    ScaledNs lastGmPhaseChange;
    double lastGmFreqChange;
};


struct PortGlobal
{
    /**
     * @brief A Boolean that is TRUE if and only if it is determined that this time-aware system and the time-aware system at the other end
     * of the link attached to this port can interoperate with each other via the IEEE 802.1AS protocol. This means that
     * a) this time-aware system is capable of executing the IEEE 802.1AS protocol,
     * b) the time-aware system at the other end of the link is capable of executing the IEEE 802.1AS protocol, and
     * c) there are no non-IEEE-802.1AS systems in between this time-aware system and the time-aware system at the other end of the link that
     * introduce sufficient impairments that the end-to-end time-synchronization performance of B.3 cannot be met.
     * The determination of asCapable is different for each medium, and is described in the respective media-dependent clauses.
     */
    bool asCapable;
    /**
     * @brief The time interval after which sync receipt timeout occurs if time synchronization information has not been received during the
     * interval. The value of syncReceiptTimeoutTimeInterval is equal to syncReceiptTimeout multiplied by the syncInterval for the port at the
     * other end of the link to which this port is attached. The value of syncInterval for the port at the other end of the link is computed
     * from logMessageInterval of the received Sync message.
     */
    UScaledNs syncReceiptTimeoutTimeInterval;
    /**
     * @brief The current value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive
     * time-synchronization event messages. This value is set in the LinkDelaySyncIntervalSetting state machine.
     */
    int8_t currentLogSyncInterval;
    /**
     * @brief The initial value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive
     * time-synchronization event messages.
     */
    int8_t initialLogSyncInterval;
    /**
     * @brief A variable containing the mean time-synchronization event message transmission interval for the port. This value is set in
     * the LinkDelaySyncIntervalSetting state machine.
     */
    UScaledNs syncInterval;
    /**
     * @brief The measured ratio of the frequency of the LocalClock entity of the time-aware system at the other end of the link attached
     * to this port, to the frequency of the LocalClock entity of this time-aware system.
     */
    double neighborRateRatio;
    /**
     * @brief The measured propagation delay on the link attached to this port, relative to the LocalClock entity of the time-aware system at
     * the other end of the link (i.e., expressed in the time base of the time-aware system at the other end of the link).
     */
    UScaledNs neighborPropDelay;
    /**
     * @brief The asymmetry in the propagation delay on the link attached to this port relative to the grandmaster time base. If propagation
     * delay asymmetry is not modeled, then delayAsymmetry is zero.
     */
    UScaledNs delayAsymmetry;
    /**
     * @brief A Boolean, set by the LinkDelaySyncIntervalSetting state machine, that indicates whether neighborRateRatio is to be computed by this port.
     */
    bool computeNeighborRateRatio;
    /**
     * @brief A Boolean, set by the LinkDelaySyncIntervalSetting state machine that indicates whether neighborPropDelay is to be computed by this port.
     */
    bool computeNeighborPropDelay;
    /**
     * @brief [the term port in the following items a) through c) is a physical port]: a Boolean that is set if the time-aware system’s MAC Relay Entity
     * and Spanning Tree Protocol Entity can use the MAC Service provided by the Port’s MAC entity to transmit and receive frames to and from the attached
     * LAN, i.e., portEnabled is TRUE if and only if:
     * a) MAC_Operational (see 6.4.2 of IEEE Std 802.1D-2004) is TRUE; and
     * b) Administrative Bridge Port State (see 14.8.2.2 of IEEE Sd 802.1D-2004) for the Port is Enabled; and
     * c) AuthControlledPortStatus is Authorized (if the port is a network access port; see IEEE Std 802.1X TM -2010 [B5]).
     */
    bool portEnabled;
    /**
     * @brief A Boolean that is set if the time-synchronization and best master selection functions of the port are enabled.
     * NOTE—It is expected that the value of pttPortEnabled will be set via the management interface (see 14.6.4). By having both portEnabled and
     * pttPortEnabled variables, a port can be enabled for data transport but not for synchronization transport.
     */
    bool pttPortEnabled;
    /**
     * @brief The portNumber of the current port.
     */
//    uint16_t thisPort;


    /**
     * @brief The value of this attribute tells a slave port the number of sync intervals to wait without receiving synchronization
     * information, before assuming that the master is no longer transmitting synchronization information, and that the BMC algorithm
     * needs to be run, if appropriate. The condition of the slave port not receiving synchronization information for syncReceiptTimeout
     * sync intervals is referred to as sync receipt timeout.
     *
     * The default value shall be 3
     */
    uint8_t syncReceiptTimeout;


    /** Media dependent... */

    /**
     * @brief The current value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Pdelay_Req messages.
     * This value is set in the LinkDelaySyncIntervalSetting state machine.
     */
    int8_t currentLogPdelayReqInterval;

    /**
     * @brief The initial value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Pdelay_Req messages.
     */
    int8_t initialLogPdelayReqInterval;

    /**
     * @brief A variable containing the mean Pdelay_Req message transmission interval for the port corresponding to this MD entity. The value is set in
     * the LinkDelaySyncIntervalSetting state machine.
     */
    UScaledNs pdelayReqInterval;

    /**
     * @brief The number of Pdelay_Req messages for which a valid response is not received, above which a port is considered to not be exchanging
     * peer delay messages with its neighbor.
     */
    uint16_t allowedLostResponses;

    /**
     * @brief A Boolean that is TRUE if the port is measuring link propagation delay. For a full-duplex, point-to-point link, the port is measuring
     * link propagation delay if it is receiving Pdelay_Resp and Pdelay_Resp_Follow_Up messages from the port at the other end of the link
     * (i.e., it performs the measurement using the peer delay mechanism).
     */
    bool isMeasuringDelay;

    /**
     * @brief The propagation time threshold, above which a port is not considered capable of participating in the IEEE 802.1AS protocol.
     * If neighborPropDelay exceeds neighborPropDelayThresh, then asCapable is set to FALSE.
     */
    UScaledNs neighborPropDelayThresh;

    /**
     * @brief The sequenceId for the next Sync message to be sent by this MD entity.
     */
    uint16_t syncSequenceId;

    /** End media dependent... */



    /** Announce */

    /**
     * @brief The time interval after which announce receipt timeout occurs if an Announce message has not been received during the interval.
     * The value of announceReceiptTimeoutTimeInterval is equal to announceReceiptTimeout (see 10.6.3.2) multiplied by the announceInterval (see 10.3.9.6)
     * for the port at the other end of the link to which this port is attached. The value of announceInterval for the port at the other end of the link is
     * computed from logMessageInterval of the received Announce message (see 10.5.2.2.11).
     */
    UScaledNs announceReceiptTimeoutTimeInterval;

    /**
     * @brief Indicates the origin and state of the port’s time-synchronization spanning tree information.
     */
    SpanningTreePortState infoIs;

    /**
     * @brief The masterPriorityVector for the port.
     */
    PriorityVector masterPriority;
    /**
     * @brief The current value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Announce messages (see 10.6.2.2).
     * This value is set in the AnnounceIntervalSetting state machine (see 10.3.14).
     */
    int8_t currentLogAnnounceInterval;

    /**
     * @brief The initial value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Announce messages (see 10.6.2.2).
     */
    int8_t initialLogAnnounceInterval;

    /**
     * @brief A variable containing the mean Announce message transmission interval for the port. This value is set in the AnnounceIntervalSetting
     * state machine (see 10.3.14).
     */
    UScaledNs announceInterval;

    /**
     * @brief The value of stepsRemoved contained in the received Announce information.
     */
    uint16_t messageStepsRemoved;

    /**
     * @brief A Boolean variable that is set to cause a port to transmit Announce information; specifically, it is set when an announce interval has elapsed
     * (see Figure 10-15), port roles have been updated, and portPriority and portStepsRemoved information has been updated with newly determined masterPriority
     * and masterStepsRemoved information.
     */
    bool newInfo;

    /**
     * @brief The portPriorityVector for the port.
     */
    PriorityVector portPriority;

    /**
     * @brief The value of stepsRemoved for the port. portStepsRemoved is set equal to masterStepsRemoved (see 10.3.8.3) after masterStepsRemoved is updated.
     */
    uint16_t portStepsRemoved;

    /**
     * @brief A pointer to a structure that contains the fields of a received Announce message.
     */
    PtpMessageAnnounce* rcvdAnnouncePtr;

    /**
     * @brief A Boolean variable that is TRUE if a received Announce message is qualified, and FALSE if it is not qualified.
     */
    bool rcvdMsg;

    /**
     * @brief A Boolean variable that is set to TRUE to indicate that the PortAnnounceInformation state machine (see 10.3.11) should copy the newly determined
     * masterPriority and masterStepsRemoved to portPriority and portStepsRemoved, respectively.
     */
    bool updtInfo;

    /**
     * @brief A global variable in which the leap61 flag (see 10.5.2.2.6) of a received Announce message is saved.
     */
    bool annLeap61;

    /**
     * @brief A global variable in which the leap59 flag (see 10.5.2.2.6) of a received Announce message is saved.
     */
    bool annLeap59;

    /**
     * @brief A global variable in which the currentUtcOffsetValid flag (see 10.5.2.2.6) of a received Announce message is saved.
     */
    bool annCurrentUtcOffsetValid;

    /**
     * @brief A global variable in which the timeTraceable flag (see 10.5.2.2.6) of a received Announce message is saved.
     */
    bool annTimeTraceable;

    /**
     * @brief A global variable in which the frequencyTraceable flag (see 10.5.2.2.6) of a received Announce message is saved.
     */
    bool annFrequencyTraceable;

    /**
     * @brief A global variable in which the currentUtcOffset field (see 10.5.3.2.1) of a received Announce message is saved.
     */
    int16_t annCurrentUtcOffset;

    /**
     * @brief a global variable in which the timeSource field (see 10.5.3.2.1) of a received Announce message is saved.
     */
    ClockTimeSource annTimeSource;

    /**
     * @brief The value of this attribute tells a slave port the number of announce intervals to wait without receiving an Announce message,
     * before assuming that the master is no longer transmitting Announce messages, and that the BMC algorithm needs to be run, if appropriate.
     * The condition of the slave port not receiving an Announce message for announceReceiptTimeout announce intervals is referred to as announce
     * receipt timeout.
     * The default value shall be 3. The announceReceiptTimeout is a per-port attribute.
     */
    uint8_t announceReceiptTimeout;

    /** End announce */


    PortIdentity identity;
};

#endif // COMM_STRUCTS

