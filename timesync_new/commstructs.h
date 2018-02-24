#ifndef COMM_STRUCTS
#define COMM_STRUCTS

#include <vector>
#include <time.h>

#include "types.h"

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

struct TimeAwareSystem
{
    /**
     * @brief A Boolean controlled by the system initialization. If BEGIN is true, all state machines,
     * including per-port state machines, continuously execute their initial state.
     */
    bool BEGIN;
    /**
     * @brief A variable containing the mean time interval between successive messages providing time-synchronization
     * information by the ClockMaster entity to the SiteSync entity. This value is given by 1000000000 × 2 clockMasterLogSyncInterval,
     * where clockMasterLogSyncInterval is the logarithm to base 2 of the mean time between the successive providing of time-synchronization
     * information by the ClockMaster entity.
     */
    UScaledNs clockMasterSyncInterval;
    /**
     * @brief The synchronized time maintained, at the slave, at the granularity of the LocalClock entity [i.e., a new value is computed every
     * localClockTickInterval by the ClockSlave entity].
     */
    ExtendedTimestamp clockSlaveTime;
    /**
     * @brief The synchronized time computed by the ClockSlave entity at the instant time- synchronization information, contained in a PortSyncSync
     * structure, is received.
     */
    ExtendedTimestamp syncReceiptTime;
    /**
     * @brief The value of currentTime (i.e., the time relative to the LocalClock entity) corresponding to syncReceiptTime.
     */
    UScaledNs syncReceiptLocalTime;
    /**
     * @brief The fractional frequency offset of the ClockSource entity frequency relative to the current grandmaster frequency.
     */
    double clockSourceFreqOffset;
    /**
     * @brief The time provided by the ClockSource entity, minus the synchronized time.
     */
    ScaledNs clockSourcePhaseOffset;
    /**
     * @brief A global variable that is set equal to the timeBaseIndicator parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    uint16_t clockSourceTimeBaseIndicator;
    /**
     * @brief A global variable that is set equal to the previous value of clockSourceTimeBaseIndicator.
     */
    uint16_t clockSourceTimeBaseIndicatorOld;
    /**
     * @brief A global variable that is set equal to the lastGmPhaseChange parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    ScaledNs clockSourceLastGmPhaseChange;
    /**
     * @brief  A global variable that is set equal to the lastGmFreqChange parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    double clockSourceLastGmFreqChange;
    /**
     * @brief  The current value of time relative to the LocalClock entity clock.
     */
//    UScaledNs currentTime;
    /**
     * @brief A Boolean that indicates whether a grandmaster-capable time-aware system is present in the domain. If TRUE, a grandmaster-capable
     * time-aware system is present; if FALSE, a grandmaster-capable time-aware system is not present.
     */
    bool gmPresent;
    /**
     * @brief The measured ratio of the frequency of the ClockSource entity to the frequency of the LocalClock entity.
     */
    double gmRateRatio;
    /**
     * @brief The most recent value of gmTimeBaseIndicator provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    uint16_t gmTimeBaseIndicator;
    /**
     * @brief The most recent value of lastGmPhaseChange provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    ScaledNs lastGmPhaseChange;
    /**
     * @brief The most recent value of lastGmFreqChange provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    double lastGmFreqChange;
    /**
     * @brief  The time interval between two successive significant instants (i.e. “ticks”) of the LocalClock entity.
     */
    TimeInterval localClockTickInterval;
    /**
     * @brief The value of currentTime when the most recent ClockSourceTime.invoke function was received from the ClockSource entity,
     * or when the LocalClock entity most recently updated its time.
     */
    UScaledNs localTime;
    /**
     * @brief Array of length numberPorts+1. selectedRole[j] is set equal to the port role of port whose portNumber is j.
     */
    std::vector<PortRole> selectedRole;
    /**
     * @brief The time maintained by the ClockMaster entity, based on information received from the ClockSource and LocalClock entities.
     */
    ExtendedTimestamp masterTime;
    /**
     * @brief The clockIdentity of the current time-aware system.
     */
    uint8_t thisClock[8];




    /**
     * @brief Specifies the mean time interval between successive instants at which the ClockMaster entity provides time-synchronization
     * information to the SiteSync entity. The value is less than or equal to the smallest currentLogSyncInterval value for all the
     * ports of the time-aware system. The clockMasterLogSyncInterval is an internal, per-time-aware system variable.
     */
    int8_t clockMasterLogSyncInterval;


    UScaledNs GetCurrentTime()
    {
        struct timespec ts;
        UScaledNs uscaled;

        clock_gettime(CLOCK_MONOTONIC, &ts);

        uscaled.ns = (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
        uscaled.ns_frac = 0;

        return uscaled;
    }
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
    uint16_t thisPort;




    /**
     * @brief The value of this attribute tells a slave port the number of sync intervals to wait without receiving synchronization
     * information, before assuming that the master is no longer transmitting synchronization information, and that the BMC algorithm
     * needs to be run, if appropriate. The condition of the slave port not receiving synchronization information for syncReceiptTimeout
     * sync intervals is referred to as sync receipt timeout.
     *
     * The default value shall be 3
     */
    uint8_t syncReceiptTimeout;
};


struct MDGlobal
{
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
};

#endif // COMM_STRUCTS

