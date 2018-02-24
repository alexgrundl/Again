#ifndef SITESYNCSYNC_H
#define SITESYNCSYNC_H

#include "commstructs.h"
#include "statemachinebase.h"
#include "portsyncsyncsend.h"
#include "clockslavesync.h"

class SiteSyncSync : public StateMachineBase
{
public:

    SiteSyncSync(TimeAwareSystem* timeAwareSystem, ClockSlaveSync* clockSlaveSync, std::vector<PortSyncSyncSend*>& portSyncSyncSends);


    virtual ~SiteSyncSync();

    /**
     * @brief Creates a PortSyncSync structure to be transmitted, and returns a pointer to this structure. The members are
     * copied from the received PortSyncSync structure pointed to by rcvdPSSyncPtr.
     * @param rcvdPSSyncIndPtr The structure to copy its members.
     * @return A Pointer to thecreated PortSyncSync structure.
     */
    PortSyncSync* SetPSSyncSend (PortSyncSync* rcvdPSSyncIndPtr);
    /**
     * @brief Transmits a copy of the PortSyncSync structure pointed to by txPSSyncPtr to the PortSyncSyncSend state machine
     * of each PortSync entity and the ClockSlaveSync state machine of the ClockSlave entity of this time-aware system.
     * @param txPSSyncPtr The structure to transmit.
     */
    void TxPSSync (PortSyncSync* txPSSyncPtr);


    void ProcessState();


    void ProcessStruct(PortSyncSync* rcvd);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when a PortSyncSync structure is received from the
     * PortSyncSyncReceive state machine of a PortSync entity or from the ClockMasterSyncSend state machine of the ClockMaster
     * entity. This variable is reset by this state machine.
     */
    bool m_rcvdPSSync;
    /**
     * @brief A pointer to the received PortSyncSync structure indicated by rcvdPSSync.
     */
    PortSyncSync* m_rcvdPSSyncPtr;
    /**
     * @brief A pointer to the PortSyncSync structure transmitted by the state machine.
     */
    PortSyncSync* m_txPSSyncPtr;


    ClockSlaveSync* m_clockSlaveSync;


    std::vector<PortSyncSyncSend*> m_portSyncSyncSends;
};

#endif // SITESYNCSYNC_H
