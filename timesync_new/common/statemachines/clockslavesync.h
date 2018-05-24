#ifndef CLOCKSLAVESYNC_H
#define CLOCKSLAVESYNC_H

#include <memory>

#include "statemachinebase.h"
#include "clockmastersyncoffset.h"

class ClockSlaveSync : public StateMachineBase
{
public:

    ClockSlaveSync(TimeAwareSystem *timeAwareSystem, ClockMasterSyncOffset* clockMasterSyncOffset);


    virtual ~ClockSlaveSync();


    void ProcessState();


    void SetPortSyncSync(PortSyncSync* rcvd);

    /**
     * @brief updates the global variable clockSlaveTime , based on information received from the SiteSync and LocalClock entities.
     * It is the responsibility of the application to filter slave times appropriately (see B.3 and B.4 for examples). As one example,
     * clockSlaveTime can be incremented by localClockTickInterval divided by rateRatio member of the received PortSyncSync structure.
     * If no time-aware system is grandmaster-capable, i.e., gmPresent is FALSE, then clockSlaveTime is set to the time provided by the LocalClock.
     * This function is invoked when rcvdLocalClockTick is TRUE.
     */
    void UpdateSlaveTime();

    /**
     * @brief Invokes the application interfacefunction whose name is functionName. In the case here, functionName is clockTargetPhaseDiscontinuity.result.
     * @param functionName
     */
    void InvokeApplicationInterfaceFunction (void* functionName);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when a PortSyncSync structure is received from the
     * SiteSyncSync state machine of the SiteSync entity. This variable is reset by this state machine.
     */
    bool m_rcvdPSSync;
    /**
     * @brief A Boolean variable that notifies the current state machine when the LocalClock entity updates its time.
     * This variable is reset by this state machine.
     */
    bool m_rcvdLocalClockTick;
    /**
     * @brief A pointer to the received PortSyncSync structure.
     */
    PortSyncSync* m_rcvdPSSyncPtr;


    ClockMasterSyncOffset* m_clockMasterSyncOffset;
};

#endif // CLOCKSLAVESYNC_H
