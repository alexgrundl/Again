#ifndef TIMEAWARESYSTEM_H
#define TIMEAWARESYSTEM_H

#include <time.h>
#include <vector>
#include <string.h>
#include <ptpclock.h>

#include "types.h"

class TimeAwareSystem
{
public:

    TimeAwareSystem();


    ~TimeAwareSystem();


    void AddPath(uint8_t* path);


    void ClearPathTrace();


    std::vector<uint8_t*> GetPathTrace();

// private:
        /**
         * @brief A Boolean controlled by the system initialization. If BEGIN is true, all state machines,
         * including per-port state machines, continuously execute their initial state.
         */
        bool BEGIN;
        /**
         * @brief A variable containing the mean time interval between successive messages providing time-synchronization
         * information by the ClockMaster entity to the SiteSync entity. This value is given by 1000000000 × 2^BclockMasterLogSyncInterval,
         * where clockMasterLogSyncInterval is the logarithm to base 2 of the mean time between the successive providing of time-synchronization
         * information by the ClockMaster entity.
         */
//        UScaledNs clockMasterSyncInterval;
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


        UScaledNs GetCurrentTime();


        /** Announce */

        /**
         * @brief A Boolean array of length numberPorts (see 8.6.1). Setting reselect[j], where 0 ≤ j ≤ numberPorts, to TRUE causes the ROLE_SELECTION
         * block of the PortRoleSelection state machine (see 10.3.12) to be re-entered, which in turn causes the port role of each port of the
         * time-aware system to be updated (via the function updateRolesTree(), see 10.3.12.1.4).
         */
        std::vector<bool> reselect;

        /**
         * @brief A Boolean array of length numberPorts (see 8.6.1). selected[j], where 0 ≤ j ≤ numberPorts, is set to TRUE immediately after the port
         * roles of all the ports are updated. This indicates to the PortAnnounceInformation state machine (see 10.3.11) that it can update the
         * portPriorityVector and other variables for each port.
         */
        std::vector<bool> selected;

        /**
         * @brief The value of stepsRemoved for the time-aware system, after the port roles of all the ports have been updated
         * (see 10.3.12.1.4 for details on the computation of masterStepsRemoved).
         */
        uint8_t masterStepsRemoved;
        /**
         * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the current grandmaster, contains 61 s,
         * and FALSE if the last minute of the current UTC day does not contain 61 s.
         */
        bool leap61;

        /**
         * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the current grandmaster, contains 59 s,
         * and FALSE if the last minute of the current UTC day does not contain 59 s.
         */
        bool leap59;

        /**
         * @brief A Boolean variable whose value is TRUE if currentUtcOffset (see 10.3.8.9), relative to the current grandmaster, is known to be correct,
         * and FALSE if currentUtcOffset is not known to be correct.
         */
        bool currentUtcOffsetValid;

        /**
         * @brief A Boolean variable whose value is TRUE if both clockSlaveTime (i.e., the synchronized time maintained at the slave, see 10.2.3.3)
         * and currentUtcOffset (see 10.3.8.9) relative to the current grandmaster are traceable to a primary reference, and FALSE if one or both are
         * not traceable to a primary reference.
         */
        bool timeTraceable;

        /**
         * @brief A Boolean variable whose value is TRUE if the frequency that determines clockSlaveTime, i.e., the frequency of the LocalClockEntity
         * multiplied by the most recently computed rateRatio by the PortSyncSyncReceive state machine (see 10.2.7.1.4), is traceable to a primary reference,
         * and FALSE if this frequency is not traceable to a primary reference.
         */
        bool frequencyTraceable;

        /**
         * @brief The difference between TAI time and UTC time, i.e., TAI time minus UTC time, expressed in seconds, and relative to the current grandmaster.
         * NOTE—For example, 2006-01-01 00:00:00 UTC and 2006-01-01 00:00:33 TAI represent the same instant of time. At this time, currentUtcOffset was equal
         * to 33 s.
         */
        int16_t currentUtcOffset;

        /**
         * @brief The value of the timeSource attribute of the current grandmaster (see 8.6.2.7).
         */
        ClockTimeSource timeSource;

        /**
         * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the ClockMaster entity of this time-aware system,
         * contains 61 s, and FALSE if the last minute of the current UTC day does not contain 61 s.
         */
        bool sysLeap61;

        /**
         * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the ClockMaster entity of this time-aware system,
         * contains 59 s, and FALSE if the last minute of the current UTC day does not contain 59 s.
         */
        bool sysLeap59;

        /**
         * @brief a Boolean variable whose value is TRUE if currentUtcOffset (see 10.3.8.9), relative to the ClockMaster entity of this time-aware system,
         * is known to be correct, and FALSE if currentUtcOffset is not known to be correct.
         */
        bool sysCurrentUTCOffsetValid;

        /**
         * @brief A Boolean variable whose value is TRUE if both masterTime (i.e., the time maintained by the ClockMaster entity of this time-aware system,
         * see 10.2.3.21) and currentUtcOffset (see 10.3.8.9) relative to the ClockMaster entity of this time-aware system, are traceable to a primary reference,
         * and FALSE if one or both are not traceable to a primary reference.
         */
        bool sysTimeTraceable;

        /**
         * @brief A Boolean variable whose value is TRUE if the frequency that determines masterTime of the ClockMaster entity of this time-aware system, i.e.,
         * the frequency of the LocalClockEntity multiplied by the most recently computed gmRateRatio by the ClockMasterSyncReceive state machine
         * (see 10.2.3.14 and 10.2.10), is traceable to a primary reference, and FALSE if this frequency is not traceable to a primary reference.
         */
        bool sysFrequencyTraceable;

        /**
         * @brief The difference between TAI time and UTC time, i.e., TAI time minus UTC time, expressed in seconds, and relative to the ClockMaster entity of
         * this time-aware system. The data type for currentUtcOffset is Integer16.
         * NOTE—See the NOTE in 10.3.8.9 for more detail on the sign convention.
         */
        int16_t sysCurrentUtcOffset;

        /**
         * @brief The value of the timeSource attribute of the ClockMaster entity of this time-aware system (see 8.6.2.7).
         */
        ClockTimeSource sysTimeSource;

        /**
         * @brief The systemPriority vector for this time-aware system.
         */
        PriorityVector systemPriority;

        /**
         * @brief The current gmPriorityVector for the time-aware system.
         */
        PriorityVector gmPriority;

        /**
         * @brief The previous gmPriorityVector for the time-aware system, prior to the most
        recent invocation of the PortRoleSelection state machine.
         */
        PriorityVector lastGmPriority;


        /** End announce. */

private:

        /**
         * @brief An array that contains the clockIdentities of the successive time-aware systems that receive, process, and send Announce messages.
         * The data type for pathTrace is ClockIdentity[N], where N is the number of time-aware systems, including the grandmaster, that the Announce
         * information has traversed.
         * NOTE—N is equal to stepsRemoved+1 (see 10.5.3.2.6). The size of the pathTrace array can change after each reception of an Announce message.
         */
        std::vector<uint8_t*> pathTrace;

        PtpClock clockSource;
};

#endif // TIMEAWARESYSTEM_H
