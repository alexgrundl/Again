#ifndef CLOCKMASTERSYNCRECEIVE_H
#define CLOCKMASTERSYNCRECEIVE_H

#include <memory>

#include "statemachinebase.h"
#include "interfaces.h"

class ClockMasterSyncReceive : public StateMachineBase
{
public:

    ClockMasterSyncReceive(TimeAwareSystem* timeAwareSystem);


    virtual ~ClockMasterSyncReceive();


    void ProcessState();


    void SignalLocalClockUpdate();


    void SetClockSourceRequest(ClockSourceTimeParams* clockSourceReqPtr);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when ClockSourceTime.invoke function is received
     * from the Clock source entity. This variable is reset by this state machine.
     */
    bool m_rcvdClockSourceReq;
    /**
     * @brief A pointer to the received ClockSourceTime.invoke function parameters.
     */
    ClockSourceTimeParams* m_rcvdClockSourceReqPtr;
    /**
     * @brief A Boolean variable that notifies the current state machine when the LocalClock entity updates its time.
     * This variable is reset by this state machine.
     */
    bool m_rcvdLocalClockTick;


    ExtendedTimestamp m_sourceTimeOld;


    UScaledNs m_localTimeOld;


    /**
     * @brief Computes gmRateRatio, using values of sourceTime conveyed by successive ClockSourceTime.invoke functions,
     * and corresponding values of localTime). Any scheme that uses this information, along with any other information
     * conveyed by the successive ClockSourceTime.invoke functions and corresponding values of localTime, to compute
     * gmRateRatio is acceptable as long as the performance requirements specified in B.2.4 are met.
     */
    void ComputeGmRateRatio();

    /**
     * @brief Updates the global variable masterTime, based on information received from the ClockSource and LocalClock
     * entities. It is the responsibility of the application to filter master times appropriately. As one example,
     * masterTime can be set equal to the sourceTime member of the ClockSourceTime.invoke function when this function
     * is received, and can be incremented by localClockTickInterval divided by gmRateRatio when rcvdLocalClockTick is TRUE.
     */
    void UpdateMasterTime();
};

#endif // CLOCKMASTERSYNCRECEIVE_H
