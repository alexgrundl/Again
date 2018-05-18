#ifndef INTERFACES
#define INTERFACES

#include "types.h"

struct ClockSourceTimeParams
{
    /**
     * @brief The value of sourceTime is the time this function is invoked by the ClockSource entity.
     */
    ExtendedTimestamp sourceTime;

    /**
     * @brief The timeBaseIndicator is a binary value that is set by the ClockSource entity. The ClockSource entity
     * changes the value whenever its time base changes. The ClockSource entity shall change the value of timeBaseIndicator
     * if and only if there is a phase or frequency change.
     */
    uint16_t timeBaseIndicator;

    /**
     * @brief The value of lastGmPhaseChange is the phase change (i.e., change in sourceTime) that occurred on the most
     * recent change in timeBaseIndicator. The value is initialized to 0.
     */
    ScaledNs lastGmPhaseChange;

    /**
     * @brief The value of lastGmFreqChange is the fractional frequency change (i.e., frequency change expressed as a pure fraction)
     * that occurred on the most recent change in timeBaseIndicator. The value is initialized to 0.
     */
    double lastGmFreqChange;
};

/**
 * @brief This interface is used by the ClockSource entity to provide time to the ClockMaster entity of a time-aware system.
 * The ClockSource entity invokes the ClockSourceTime.invoke function to provide the time, relative to the ClockSource, that
 * this function is invoked.
 */
class ClockSourceTime
{
public:

    /**
     * @brief Provides the time, relative to the ClockSource, that this function is invoked.
     * @param params
     */
    virtual void Invoke(ClockSourceTimeParams* params) = 0;
};

/**
 * @brief This interface is used by the ClockTarget entity to request the synchronized time of an event that it signals to
 * the ClockSlave entity of a time-aware system. The ClockTarget entity invokes the ClockTargetEventCapture.invoke function
 * to signal an event to the ClockSlave entity. The ClockSlave entity invokes the ClockTargetEventCapture.result function to
 * return the time of the event relative to the current grandmaster or, if no time-aware system is grandmaster-capable,
 * the LocalClock. The ClockTargetEventCapture.result function also returns gmPresent, to indicate to the ClockTarget whether
 * or not a grandmaster is present.
 */
class ClockTargetEventCapture
{
public:

    virtual void Invoke() = 0;

    /**
     * @brief Result
     * @param slaveTimeCallback The time, relative to the grandmaster, that the corresponding ClockTargetEventCapture.invoke
     * function is invoked.
     * @param gmPresent The value of gmPresent is set equal to the value of the global variable gmPresent. This parameter
     * indicates to the ClockTarget whether or not a grandmaster is present.
     */
    virtual void Result(ExtendedTimestamp slaveTimeCallback, bool gmPresent) = 0;
};


/**
 * @brief This interface is used by the ClockTarget entity to request that the ClockSlave entity send a result at a specified
 * time relative to the grandmaster. The ClockTarget entity invokes the ClockTargetTriggerGenerate.invoke function to indicate
 * the synchronized time of the event. The ClockSlave entity invokes the ClockTargetTriggerGenerate.result function to either
 * signal the event at the requested synchronized time or indicate an error condition.
 */
class ClockTargetTriggerGenerate
{
public:

    /**
     * @brief Invoke
     * @param slaveTimeCallback If  nonzero, its value is the synchronized time the corresponding ClockTargetTriggerGenerate.result
     * function, i.e., the trigger, is to be invoked. If slaveTimeCallback is zero, any previous ClockTargetTriggerGenerate.invoke
     * function for which a ClockTargetTriggerGenerate.result function has not yet been issued is canceled.
     */
    virtual void Invoke(ExtendedTimestamp slaveTimeCallback) = 0;

    /**
     * @brief Result
     * @param errorCondition A value of FALSE indicates that the ClockTargetTriggerGenerate.result function was invoked at the time,
     * relative to the grandmaster, contained in the corresponding ClockTargetTriggerGenerate.invoke function. A value of TRUE
     * indicates that the ClockTargetTriggerGenerate.result function could not be invoked at the synchronized time contained in the
     * corresponding ClockTargetTriggerGenerate.invoke function.
     * @param gmPresent The value of gmPresent is set equal to the value of the global variable gmPresent. This parameter indicates
     * to the ClockTarget whether or not a grandmaster is present.
     */
    virtual void Result(bool errorCondition, bool gmPresent) = 0;
};



class ClockTargetClockGenerator
{
public:

    virtual void Invoke(TimeInterval clockPeriod, ExtendedTimestamp slaveTimeCallbackPhase) = 0;

    virtual void Result(ExtendedTimestamp slaveTimeCallback) = 0;
};


class ClockTargetPhaseDiscontinuity
{
public:

    virtual void Result(uint8_t* gmIdentity, uint16_t gmTimeBaseIndicator, ScaledNs lastGmPhaseChange, double lastGmFreqChange) = 0;
};

#endif // INTERFACES
