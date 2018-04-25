#ifndef TIMEAWARESYSTEM_H
#define TIMEAWARESYSTEM_H

#include <time.h>
#include <vector>
#include <string.h>
#include <ptpclock.h>

#include "types.h"
#include "linuxsharedmemoryipc.h"

class TimeAwareSystem
{
public:

    TimeAwareSystem();


    ~TimeAwareSystem();


    /**
     * @brief A Boolean controlled by the system initialization. If BEGIN is true, all state machines,
     * including per-port state machines, continuously execute their initial state.
     */
    bool BEGIN;


    void AddPath(const uint8_t* path);


    void ClearPathTrace();


    std::vector<uint8_t*> GetPathTrace();

    /**
     * @brief A variable containing the mean time interval between successive messages providing time-synchronization
     * information by the ClockMaster entity to the SiteSync entity. This value is given by 1000000000 × 2^BclockMasterLogSyncInterval,
     * where clockMasterLogSyncInterval is the logarithm to base 2 of the mean time between the successive providing of time-synchronization
     * information by the ClockMaster entity.
     */
//      UScaledNs clockMasterSyncInterval;


    ExtendedTimestamp GetClockSlaveTime();


    void SetClockSlaveTime(ExtendedTimestamp time);


    ExtendedTimestamp GetSyncReceiptTime();


    void SetSyncReceiptTime(ExtendedTimestamp time);


    UScaledNs GetSyncReceiptLocalTime();


    void SetSyncReceiptLocalTime(UScaledNs time);


    double GetClockSourceFreqOffset();


    void SetClockSourceFreqOffset(double offset);


    ScaledNs GetClockSourcePhaseOffset();


    void SetClockSourcePhaseOffset(ScaledNs offset);


    uint16_t GetClockSourceTimeBaseIndicator();


    void SetClockSourceTimeBaseIndicator(uint16_t indicator);


    uint16_t GetClockSourceTimeBaseIndicatorOld();


    void SetClockSourceTimeBaseIndicatorOld(uint16_t indicator);


    ScaledNs GetClockSourceLastGmPhaseChange();


    void SetClockSourceLastGmPhaseChange(ScaledNs value);


    double GetClockSourceLastGmFreqChange();


    void SetClockSourceLastGmFreqChange(double value);

    /**leap61
     * @brief  The current value of time relative to the LocalClock entity clock.
     */
    UScaledNs ReadCurrentTime();


    bool IsGmPresent();


    void SetGmPresent(bool present);


    double GetGmRateRatio();


    void SetGmRateRatio(double ratio);


    uint16_t GetGmTimeBaseIndicator();


    void SetGmTimeBaseIndicator(uint16_t indicator);


    ScaledNs GetLastGmPhaseChange();


    void SetLastGmPhaseChange(ScaledNs value);


    double GetLastGmFreqChange();


    void SetLastGmFreqChange(double value);


    TimeInterval GetLocalClockTickInterval();


    void SetLocalClockTickInterval(TimeInterval interval);


    UScaledNs GetLocalTime();


    void SetLocalTime(UScaledNs time);


    void AddSelectedRole(PortRole role);


    PortRole GetSelectedRole(int index);


    void SetSelectedRole(int index, PortRole role);


    ExtendedTimestamp GetMasterTime();


    void SetMasterTime(ExtendedTimestamp time);


    void IncreaseMasterTime(TimeInterval interval);


    const uint8_t* GetClockIdentity();


    void SetClockIdentity(uint8_t* identity);


    int8_t GetClockMasterLogSyncInterval();


    void SetClockMasterLogSyncInterval(int8_t interval);



    //Announce


    uint8_t GetMasterStepsRemoved();


    void SetMasterStepsRemoved(uint8_t stepsRemoved);


    bool GetLeap61();


    void SetLeap61(bool enable);


    bool GetLeap59();


    void SetLeap59(bool enable);


    bool GetCurrentUtcOffsetValid();


    void SetCurrentUtcOffsetValid(bool enable);


    bool GetTimeTraceable();


    void SetTimeTraceable(bool enable);


    bool GetFrequencyTraceable();


    void SetFrequencyTraceable(bool enable);


    int16_t GetCurrentUtcOffset();


    void SetCurrentUtcOffset(int16_t offset);


    ClockTimeSource GetTimeSource();


    void SetTimeSource(ClockTimeSource m_timeSource);


    bool GetSysLeap61();


    void SetSysLeap61(bool enable);


    bool GetSysLeap59();


    void SetSysLeap59(bool enable);


    bool GetSysCurrentUTCOffsetValid();


    void SetSysCurrentUTCOffsetValid(bool valid);


    bool GetSysTimeTraceable();


    void SetSysTimeTraceable(bool traceable);


    bool GetSysFrequencyTraceable();


    void SetSysFrequencyTraceable(bool traceable);


    int16_t GetSysCurrentUtcOffset();


    void SetSysCurrentUtcOffset(int16_t offset);


    ClockTimeSource GetSysTimeSource();


    void SetSysTimeSource(ClockTimeSource timeSource);


    PriorityVector GetSystemPriority();


    void SetSystemPriority(PriorityVector priority);


    PriorityVector GetGmPriority();


    void SetGmPriority(PriorityVector priority);


    PriorityVector GetLastGmPriority();


    void SetLastGmPriority(PriorityVector priority);

    //End announce


    void InitLocalClock(PtpClock* clock, int clockIndex);


    PtpClock* GetLocalClock();


    uint8_t GetDomain();


    void SetDomain(uint8_t domain);


    static uint8_t GetDomainToSyntonize();


    static void SetDomainToSyntonize(uint8_t domain);


    static uint8_t GetDomainToMeasurePDelay();


    static void SetDomainToMeasurePDelay(uint8_t domain);

private:

    /**
     * @brief The synchronized time maintained, at the slave, at the granularity of the LocalClock entity [i.e., a new value is computed every
     * localClockTickInterval by the ClockSlave entity].
     */
    ExtendedTimestamp m_clockSlaveTime;

    /**
     * @brief The synchronized time computed by the ClockSlave entity at the instant time- synchronization information, contained in a PortSyncSync
     * structure, is received.
     */
    ExtendedTimestamp m_syncReceiptTime;

    /**
     * @brief The value of currentTime (i.e., the time relative to the LocalClock entity) corresponding to syncReceiptTime.
     */
    UScaledNs m_syncReceiptLocalTime;

    /**
     * @brief The fractional frequency offset of the ClockSource entity frequency relative to the current grandmaster frequency.
     */
    double m_clockSourceFreqOffset;

    /**
     * @brief The time provided by the ClockSource entity, minus the synchronized time.
     */
    ScaledNs m_clockSourcePhaseOffset;

    /**
     * @brief A global variable that is set equal to the timeBaseIndicator parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    uint16_t m_clockSourceTimeBaseIndicator;

    /**
     * @brief A global variable that is set equal to the previous value of clockSourceTimeBaseIndicator.
     */
    uint16_t m_clockSourceTimeBaseIndicatorOld;

    /**
     * @brief A global variable that is set equal to the lastGmPhaseChange parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    ScaledNs m_clockSourceLastGmPhaseChange;

    /**
     * @brief  A global variable that is set equal to the lastGmFreqChange parameter of the ClockSourceTime.invoke application interface function.
     * That parameter is set by the ClockSource entity and is changed by that entity whenever the time base changes.
     */
    double m_clockSourceLastGmFreqChange;

    /**
     * @brief A Boolean that indicates whether a grandmaster-capable time-aware system is present in the domain. If TRUE, a grandmaster-capable
     * time-aware system is present; if FALSE, a grandmaster-capable time-aware system is not present.
     */
    bool m_gmPresent;

    /**
     * @brief The measured ratio of the frequency of the ClockSource entity to the frequency of the LocalClock entity.
     */
    double m_gmRateRatio;

    /**
     * @brief The most recent value of gmTimeBaseIndicator provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    uint16_t m_gmTimeBaseIndicator;

    /**
     * @brief The most recent value of lastGmPhaseChange provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    ScaledNs m_lastGmPhaseChange;

    /**
     * @brief The most recent value of lastGmFreqChange provided to the ClockSlaveSync state machine via a PortSyncSync structure.
     */
    double m_lastGmFreqChange;

    /**
     * @brief  The time interval between two successive significant instants (i.e. “ticks”) of the LocalClock entity.
     */
    TimeInterval m_localClockTickInterval;

    /**
     * @brief The value of currentTime when the most recent ClockSourceTime.invoke function was received from the ClockSource entity,
     * or when the LocalClock entity most recently updated its time.
     */
    UScaledNs m_localTime;

    /**
     * @brief Array of length numberPorts+1. selectedRole[j] is set equal to the port role of port whose portNumber is j.
     */
    std::vector<PortRole> m_selectedRole;

    /**
     * @brief The time maintained by the ClockMaster entity, based on information received from the ClockSource and LocalClock entities.
     */
    ExtendedTimestamp m_masterTime;

    /**
     * @brief The clockIdentity of the current time-aware system.
     */
    uint8_t m_thisClock[CLOCK_ID_LENGTH];

    /**
     * @brief Specifies the mean time interval between successive instants at which the ClockMaster entity provides time-synchronization
     * information to the SiteSync entity. The value is less than or equal to the smallest currentLogSyncInterval value for all the
     * ports of the time-aware system. The clockMasterLogSyncInterval is an internal, per-time-aware system variable.
     */
    int8_t m_clockMasterLogSyncInterval;

    /**
     * @brief An array that contains the clockIdentities of the successive time-aware systems that receive, process, and send Announce messages.
     * The data type for pathTrace is ClockIdentity[N], where N is the number of time-aware systems, including the grandmaster, that the Announce
     * information has traversed.
     * NOTE—N is equal to stepsRemoved+1 (see 10.5.3.2.6). The size of the pathTrace array can change after each reception of an Announce message.
     */
    std::vector<uint8_t*> m_pathTrace;


    //Announce

    /**
     * @brief The value of stepsRemoved for the time-aware system, after the port roles of all the ports have been updated
     * (see 10.3.12.1.4 for details on the computation of masterStepsRemoved).
     */
    uint8_t m_masterStepsRemoved;

    /**
     * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the current grandmaster, contains 61 s,
     * and FALSE if the last minute of the current UTC day does not contain 61 s.
     */
    bool m_leap61;

    /**
     * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the current grandmaster, contains 59 s,
     * and FALSE if the last minute of the current UTC day does not contain 59 s.
     */
    bool m_leap59;

    /**
     * @brief A Boolean variable whose value is TRUE if currentUtcOffset (see 10.3.8.9), relative to the current grandmaster, is known to be correct,
     * and FALSE if currentUtcOffset is not known to be correct.
     */
    bool m_currentUtcOffsetValid;

    /**
     * @brief A Boolean variable whose value is TRUE if both clockSlaveTime (i.e., the synchronized time maintained at the slave, see 10.2.3.3)
     * and currentUtcOffset (see 10.3.8.9) relative to the current grandmaster are traceable to a primary reference, and FALSE if one or both are
     * not traceable to a primary reference.
     */
    bool m_timeTraceable;

    /**
     * @brief A Boolean variable whose value is TRUE if the frequency that determines clockSlaveTime, i.e., the frequency of the LocalClockEntity
     * multiplied by the most recently computed rateRatio by the PortSyncSyncReceive state machine (see 10.2.7.1.4), is traceable to a primary reference,
     * and FALSE if this frequency is not traceable to a primary reference.
     */
    bool m_frequencyTraceable;


    int16_t m_currentUtcOffset;

    /**
     * @brief The value of the timeSource attribute of the current grandmaster (see 8.6.2.7).
     */
    ClockTimeSource m_timeSource;

    /**
     * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the ClockMaster entity of this time-aware system,
     * contains 61 s, and FALSE if the last minute of the current UTC day does not contain 61 s.
     */
    bool m_sysLeap61;

    /**
     * @brief A Boolean variable whose value is TRUE if the last minute of the current UTC day, relative to the ClockMaster entity of this time-aware system,
     * contains 59 s, and FALSE if the last minute of the current UTC day does not contain 59 s.
     */
    bool m_sysLeap59;

    /**
     * @brief a Boolean variable whose value is TRUE if currentUtcOffset (see 10.3.8.9), relative to the ClockMaster entity of this time-aware system,
     * is known to be correct, and FALSE if currentUtcOffset is not known to be correct.
     */
    bool m_sysCurrentUTCOffsetValid;

    /**
     * @brief A Boolean variable whose value is TRUE if both masterTime (i.e., the time maintained by the ClockMaster entity of this time-aware system,
     * see 10.2.3.21) and currentUtcOffset (see 10.3.8.9) relative to the ClockMaster entity of this time-aware system, are traceable to a primary reference,
     * and FALSE if one or both are not traceable to a primary reference.
     */
    bool m_sysTimeTraceable;

    /**
     * @brief A Boolean variable whose value is TRUE if the frequency that determines masterTime of the ClockMaster entity of this time-aware system, i.e.,
     * the frequency of the LocalClockEntity multiplied by the most recently computed gmRateRatio by the ClockMasterSyncReceive state machine
     * (see 10.2.3.14 and 10.2.10), is traceable to a primary reference, and FALSE if this frequency is not traceable to a primary reference.
     */
    bool m_sysFrequencyTraceable;

    /**
     * @brief The difference between TAI time and UTC time, i.e., TAI time minus UTC time, expressed in seconds, and relative to the ClockMaster entity of
     * this time-aware system. The data type for currentUtcOffset is Integer16.
     * NOTE—See the NOTE in 10.3.8.9 for more detail on the sign convention.
     */
    int16_t m_sysCurrentUtcOffset;

    /**
     * @brief The value of the timeSource attribute of the ClockMaster entity of this time-aware system (see 8.6.2.7).
     */
    ClockTimeSource m_sysTimeSource;

    /**
     * @brief The systemPriority vector for this time-aware system.
     */
    PriorityVector m_systemPriority;

    /**
     * @brief The current gmPriorityVector for the time-aware system.
     */
    PriorityVector m_gmPriority;

    /**
     * @brief The previous gmPriorityVector for the time-aware system, prior to the most
    recent invocation of the PortRoleSelection state machine.
     */
    PriorityVector m_lastGmPriority;

    //End announce


    PtpClock* m_clockLocal;


    uint8_t m_domain;


    static uint8_t s_domainToSyntonize;


    static uint8_t s_domainToMeasurePDelay;
};

#endif // TIMEAWARESYSTEM_H
