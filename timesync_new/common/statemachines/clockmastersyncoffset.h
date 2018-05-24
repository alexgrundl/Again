#ifndef CLOCKMASTERSYNCOFFSET_H
#define CLOCKMASTERSYNCOFFSET_H

#include "statemachinebase.h"
#include "timecontrol.h"

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


    void SignalSyncReceiptTimeReceive();

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when syncReceiptTime is received from
     * the ClockSlave entity. This variable is reset by this state machine.
     */
    bool m_rcvdSyncReceiptTime;


    UScaledNs m_lastSyncReceiptLocalTime;


    ExtendedTimestamp m_lastSyncReceiptTime;


    TimeControl m_timeControl;
};

#endif // CLOCKMASTERSYNCOFFSET_H
