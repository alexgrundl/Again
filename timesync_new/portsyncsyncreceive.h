#ifndef PORTSYNCSYNCRECEIVE_H
#define PORTSYNCSYNCRECEIVE_H

#include <math.h>

#include "statemachinebase.h"
#include "sitesyncsync.h"

class PortSyncSyncReceive : public StateMachineBasePort
{
public:

    PortSyncSyncReceive(TimeAwareSystem* timeAwareSystem, PortGlobal* port, SiteSyncSync* siteSyncSync);


    ~PortSyncSyncReceive();


    void ProcessState();

    /**
     * @brief Creates a PortSyncSync structure to be transmitted, and returns a pointer to this structure.
     * @param rcvdMDSyncPtr
     * @param syncReceiptTimeoutTimeInterval
     * @param rateRatio
     */
    PortSyncSync* SetPSSyncPSSR (MDSyncReceive* rcvdMDSyncPtr, UScaledNs syncReceiptTimeoutTimeInterval, double rateRatio);

    /**
     * @brief Transmits a copy of the PortSyncSync structure pointed to by txPSSyncPtr to the SiteSyncSync
     * state machine of this time-aware system.
     * @param txPSSyncPtr The structure to transmit.
     */
    void TxPSSyncPSSR (PortSyncSync* txPSSyncPtr);


    void ProcessStruct(MDSyncReceive* rcvd);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when an MDSyncReceive structure is received
     * from the MDSyncReceiveSM state machine of an MD entity of the same port. This variable is reset by this state machine.
     */
    bool m_rcvdMDSync;
    /**
     * @brief A pointer to the received MDSyncReceive structure indicated by rcvdMDSync.
     */
    MDSyncReceive* m_rcvdMDSyncPtr;
    /**
     * @brief A pointer to the PortSyncSync structure transmitted by the state machine.
     */
    PortSyncSync* m_txPSSyncPtr;
    /**
     * @brief A Double variable that holds the ratio of the frequency of the grandmaster to the frequency of the LocalClock entity.
     * This frequency ratio is computed by (a) measuring the ratio of the grandmaster frequency to the LocalClock frequency at the
     * grandmaster time-aware system and initializing rateRatio to this value in the ClockMasterSend state machine of the grandmaster
     * node, and (b) accumulating, in the PortSyncSyncReceive state machine of each time-aware system, the frequency offset of the
     * LocalClock entity of the time-aware system at the remote end of the link attached to that port to the frequency of the LocalClock
     * entity of this time-aware system.
     */
    double m_rateRatio;


    SiteSyncSync* m_siteSyncSync;
};

#endif // PORTSYNCSYNCRECEIVE_H
