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

class ClockTargetEventCapture
{
public:

    virtual void Invoke() = 0;

    virtual void Result(ExtendedTimestamp slaveTimeCallback, bool gmPresent) = 0;
};

class ClockTargetTriggerGenerate
{
public:

    virtual void Invoke(ExtendedTimestamp slaveTimeCallback) = 0;

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
