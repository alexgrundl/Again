#ifndef CLOCKMASTERSYNCOFFSET_H
#define CLOCKMASTERSYNCOFFSET_H

#include "statemachinebase.h"

class ClockMasterSyncOffset : public StateMachineBase
{
public:

    ClockMasterSyncOffset(TimeAwareSystem* timeAwareSystem);


    virtual ~ClockMasterSyncOffset();

    /**
     * @brief Computes clockSourceFreqOffset, using successive values of masterTime computed by the ClockMasterSyncReceive
     * state machine and successive values of syncReceiptTime computed by the ClockSlaveSync state machine.
     * @return
     */
    double ComputeClockSourceFreqOffset();


    void ProcessState();

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when syncReceiptTime is received from
     * the ClockSlave entity. This variable is reset by this state machine.
     */
    bool m_rcvdSyncReceiptTime;
};

#endif // CLOCKMASTERSYNCOFFSET_H
