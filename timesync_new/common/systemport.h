#ifndef SYSTEMPORT_H
#define SYSTEMPORT_H

#define __STDC_LIMIT_MACROS
#include "types.h"
#include "ptpmessageannounce.h"

/**
 * @brief Class representing one port of a time aware system.
 */
class SystemPort
{
public:

    SystemPort();

    virtual ~SystemPort();


    void ResetPortPriority();


    bool GetAsCapable();


    void SetAsCapable(bool capable);


    UScaledNs GetSyncReceiptTimeoutTimeInterval();


    void SetSyncReceiptTimeoutTimeInterval(UScaledNs interval);


    int8_t GetCurrentLogSyncInterval();


    void SetCurrentLogSyncInterval(int8_t interval);


    int8_t GetInitialLogSyncInterval();


    void SetInitialLogSyncInterval(int8_t interval);


    UScaledNs GetSyncInterval();


    void SetSyncInterval(UScaledNs interval);


    double GetNeighborRateRatio();


    void SetNeighborRateRatio(double ratio);


    UScaledNs GetNeighborPropDelay();


    void SetNeighborPropDelay(UScaledNs delay);


    UScaledNs GetDelayAsymmetry();


    void SetDelayAsymmetry(UScaledNs asymmetry);


    bool GetComputeNeighborRateRatio();


    void SetComputeNeighborRateRatio(bool compute);


    bool GetComputeNeighborPropDelay();


    void SetComputeNeighborPropDelay(bool compute);


    bool IsPortEnabled();


    void EnablePort();


    void DisablePort();


    bool IsPttPortEnabled();


    void EnablePttPort();


    void DisablePttPort();


    uint8_t GetSyncReceiptTimeout();


    void SetSyncReceiptTimeout(uint8_t timeout);


    int8_t GetCurrentLogPdelayReqInterval();


    void SetCurrentLogPdelayReqInterval(int8_t interval);


    int8_t GetInitialLogPdelayReqInterval();


    void SetInitialLogPdelayReqInterval(int8_t interval);


    UScaledNs GetPdelayReqInterval();


    void SetPdelayReqInterval(UScaledNs interval);


    uint16_t GetAllowedLostResponses();


    void SetAllowedLostResponses(uint16_t nAllowed);


    bool GetIsMeasuringDelay();


    void SetIsMeasuringDelay(bool isMeasuring);


    UScaledNs GetNeighborPropDelayThresh();


    void SetNeighborPropDelayThresh(UScaledNs threshold);


    uint16_t GetSyncSequenceId();


    void SetSyncSequenceId(uint16_t id);


    void IncreaseSyncSequenceId();


    bool GetReselect();


    void SetReselect(bool m_reselect);


    bool IsSelected();


    void SetSelected(bool m_selected);


    UScaledNs GetAnnounceReceiptTimeoutTimeInterval();


    void SetAnnounceReceiptTimeoutTimeInterval(UScaledNs interval);


    SpanningTreePortState GetInfoIs();


    void SetInfoIs(SpanningTreePortState info);


    PriorityVector GetMasterPriority();


    void SetMasterPriority(PriorityVector priority);


    int8_t GetCurrentLogAnnounceInterval();


    void SetCurrentLogAnnounceInterval(int8_t interval);


    int8_t GetInitialLogAnnounceInterval();


    void SetInitialLogAnnounceInterval(int8_t interval);


    UScaledNs GetAnnounceInterval();


    void SetAnnounceInterval(UScaledNs interval);


    uint16_t GetMessageStepsRemoved();


    void SetMessageStepsRemoved(uint16_t stepsRemoved);


    bool GetNewInfo();


    void SetNewInfo(bool newInfo);


    PriorityVector GetPortPriority();


    void SetPortPriority(PriorityVector priority);


    uint16_t GetPortStepsRemoved();


    void SetPortStepsRemoved(uint16_t stepsRemoved);


    PtpMessageAnnounce* GetRcvdAnnouncePtr();


    void SetRcvdAnnouncePtr(PtpMessageAnnounce* ptr);


    bool GetRcvdMsg();


    void SetRcvdMsg(bool rcvd);


    bool GetUpdtInfo();


    void SetUpdtInfo(bool update);


    bool GetAnnLeap61();


    void SetAnnLeap61(bool set);


    bool GetAnnLeap59();


    void SetAnnLeap59(bool set);


    bool GetAnnCurrentUtcOffsetValid();


    void SetAnnCurrentUtcOffsetValid(bool valid);


    bool GetAnnTimeTraceable();


    void SetAnnTimeTraceable(bool tracable);


    bool GetAnnFrequencyTraceable();


    void SetAnnFrequencyTraceable(bool tracable);


    int16_t GetAnnCurrentUtcOffset();


    void SetAnnCurrentUtcOffset(int16_t offset);


    ClockTimeSource GetAnnTimeSource();


    void SetAnnTimeSource(ClockTimeSource source);


    uint8_t GetAnnounceReceiptTimeout();


    void SetAnnounceReceiptTimeout(uint8_t timeout);

    /** End announce */


    PortIdentity GetIdentity();


    void SetIdentity(PortIdentity identity);


    uint32_t GetSyncCount();


    void SetSyncCount(uint32_t count);


    void IncreaseSyncCount();


    uint32_t GetPdelayCount();


    void SetPdelayCount(uint32_t count);


    void IncreasePdelayCount();


private:

    /**
     * @brief A Boolean that is TRUE if and only if it is determined that this time-aware system and the time-aware system at the other end
     * of the link attached to this port can interoperate with each other via the IEEE 802.1AS protocol. This means that
     * a) this time-aware system is capable of executing the IEEE 802.1AS protocol,
     * b) the time-aware system at the other end of the link is capable of executing the IEEE 802.1AS protocol, and
     * c) there are no non-IEEE-802.1AS systems in between this time-aware system and the time-aware system at the other end of the link that
     * introduce sufficient impairments that the end-to-end time-synchronization performance of B.3 cannot be met.
     * The determination of asCapable is different for each medium, and is described in the respective media-dependent clauses.
     */
    bool m_asCapable;

    /**
     * @brief The time interval after which sync receipt timeout occurs if time synchronization information has not been received during the
     * interval. The value of syncReceiptTimeoutTimeInterval is equal to syncReceiptTimeout multiplied by the syncInterval for the port at the
     * other end of the link to which this port is attached. The value of syncInterval for the port at the other end of the link is computed
     * from logMessageInterval of the received Sync message.
     */
    UScaledNs m_syncReceiptTimeoutTimeInterval;
    /**
     * @brief The current value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive
     * time-synchronization event messages. This value is set in the LinkDelaySyncIntervalSetting state machine.
     */
    int8_t m_currentLogSyncInterval;
    /**
     * @brief The initial value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive
     * time-synchronization event messages.
     */
    int8_t m_initialLogSyncInterval;
    /**
     * @brief A variable containing the mean time-synchronization event message transmission interval for the port. This value is set in
     * the LinkDelaySyncIntervalSetting state machine.
     */
    UScaledNs m_syncInterval;

    /**
     * @brief The measured ratio of the frequency of the LocalClock entity of the time-aware system at the other end of the link attached
     * to this port, to the frequency of the LocalClock entity of this time-aware system.
     */
    double m_neighborRateRatio;
    /**
     * @brief The measured propagation delay on the link attached to this port, relative to the LocalClock entity of the time-aware system at
     * the other end of the link (i.e., expressed in the time base of the time-aware system at the other end of the link).
     */
    UScaledNs m_neighborPropDelay;
    /**
     * @brief The asymmetry in the propagation delay on the link attached to this port relative to the grandmaster time base. If propagation
     * delay asymmetry is not modeled, then delayAsymmetry is zupdtInfoero.
     */
    UScaledNs m_delayAsymmetry;

    /**
     * @brief A Boolean, set by the LinkDelaySyncIntervalSetting state machine, that indicates whether neighborRateRatio is to be computed by this port.
     */
    bool m_computeNeighborRateRatio;
    /**
     * @brief A Boolean, set by the LinkDelaySyncIntervalSetting state machine that indicates whether neighborPropDelay is to be computed by this port.
     */
    bool m_computeNeighborPropDelay;
    /**
     * @brief [the term port in the following items a) through c) is a physical port]: a Boolean that is set if the time-aware system’s MAC Relay Entity
     * and Spanning Tree Protocol Entity can use the MAC Service provided by the Port’s MAC entity to transmit and receive frames to and from the attached
     * LAN, i.e., portEnabled is TRUE if and only if:
     * a) MAC_Operational (see 6.4.2 of IEEE Std 802.1D-2004) is TRUE; and
     * b) Administrative Bridge Port State (see 14.8.2.2 of IEEE Sd 802.1D-2004) for the Port is Enabled; and
     * c) AuthControlledPortStatus is Authorized (if the port is a network access port; see IEEE Std 802.1X TM -2010 [B5]).
     */
    bool m_portEnabled;
    /**
     * @brief A Boolean that is set if the time-synchronization and best master selection functions of the port are enabled.
     * NOTE—It is expected that the value of pttPortEnabled will be set via the management interface (see 14.6.4). By having both portEnabled and
     * pttPortEnabled variables, a port can be enabled for data transport but not for synchronization transport.
     */
    bool m_pttPortEnabled;

    /**
     * @brief The value of this attribute tells a slave port the number of sync intervals to wait without receiving synchronization
     * information, before assuming that the master is no longer transmitting synchronization information, and that the BMC algorithm
     * needs to be run, if appropriate. The condition of the slave port not receiving synchronization information for syncReceiptTimeout
     * sync intervals is referred to as sync receipt timeout.
     *
     * The default value shall be 3
     */
    uint8_t m_syncReceiptTimeout;


    /** Media dependent... */

    /**
     * @brief The current value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Pdelay_Req messages.
     * This value is set in the LinkDelaySyncIntervalSetting state machine.
     */
    int8_t m_currentLogPdelayReqInterval;

    /**
     * @brief The initial value of the logarithm to base 2 of the mean time interval, in seconds, between the sending of successive Pdelay_Req messages.
     */
    int8_t m_initialLogPdelayReqInterval;

    /**
     * @brief A variable containing the mean Pdelay_Req message transmission interval for the port corresponding to this MD entity. The value is set in
     * the LinkDelaySyncIntervalSetting state machine.
     */
    UScaledNs m_pdelayReqInterval;

    /**
     * @brief The number of Pdelay_Req messages for which a valid response is not received, above which a port is considered to not be exchanging
     * peer delay messages with its neighbor.
     */
    uint16_t m_allowedLostResponses;

    /**
     * @brief A Boolean that is TRUE if the port is measuring link propagation delay. For a full-duplex, point-to-point link, the port is measuring
     * link propagation delay if it is receiving Pdelay_Resp and Pdelay_Resp_Follow_Up messages from the port at the other end of the link
     * (i.e., it performs the measurement using the peer delay mechanism).
     */
    bool m_isMeasuringDelay;

    /**
     * @brief The propagation time threshold, above which a port is not considered capable of participating in the IEEE 802.1AS protocol.
     * If neighborPropDelay exceeds neighborPropDelayThresh, then asCapable is set to FALSE.
     */
    UScaledNs m_neighborPropDelayThresh;

    /**
     * @brief The sequenceId for the next Sync message to be sent by this MD entity.
     */
    uint16_t m_syncSequenceId;

    /** End media dependent... */


    /** Announce */

    /**
     * @brief Setting to TRUE causes the ROLE_SELECTION block of the PortRoleSelection state machine (see 10.3.12) to be re-entered,
     * which in turn causes the port role of the port of the time-aware system to be updated (via the function updateRolesTree(), see 10.3.12.1.4).
     */
    bool m_reselect;

    /**
     * @brief Set to TRUE immediately after the port role of the port is updated. This indicates to the PortAnnounceInformation
     * state machine (see 10.3.11) that it can update the portPriorityVector and other variables for the port.
     */
    bool m_selected;

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


    uint32_t syncCount;


    uint32_t pdelayCount;
};

#endif // SYSTEMPORT_H
