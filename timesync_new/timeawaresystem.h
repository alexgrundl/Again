#ifndef TIMEAWARESYSTEM_H
#define TIMEAWARESYSTEM_H

#include <time.h>
#include <vector>
#include <string.h>

#include "types.h"

class TimeAwareSystem
{
public:
    TimeAwareSystem();


// private:
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


        UScaledNs GetCurrentTime();
};

#endif // TIMEAWARESYSTEM_H
