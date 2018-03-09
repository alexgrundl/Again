#ifndef CLOCKMASTERSYNCSEND_H
#define CLOCKMASTERSYNCSEND_H

#include "statemachinebase.h"
#include "sitesyncsync.h"

class ClockMasterSyncSend : public StateMachineBase
{
public:


    ClockMasterSyncSend(TimeAwareSystem* timeAwareSystem, SiteSyncSync* siteSyncSync);


    virtual ~ClockMasterSyncSend();


    void ProcessState();

private:

    /**
     * @brief The time in seconds, relative to the LocalClock entity, when synchronization information will next be sent to
     * the SiteSync entity, via a PortSyncSync structure. The data type for syncSendTime is UScaledNs.
     */
    UScaledNs m_syncSendTime;
    /**
     * @brief A pointer to the PortSyncSync structure transmitted by the state machine.
     */
    PortSyncSync* m_txPSSyncPtr;


    SiteSyncSync* m_siteSyncSync;


    /**
     * @brief Creates a PortSyncSync structure to be transmitted, and returns a pointer to this structure.
     * @param gmRateRatio
     * @return A pointer to the structure to be transmitted.
     */
    void SetPSSyncCMSS(double gmRateRatio);

    /**
     * @brief Transmits a copy of the PortSyncSync structure pointed to by txPSSyncPtr to the SiteSync state machine.
     * @param txPSSyncPtr The structure to transmit.
     */
    void TxPSSyncCMSS();
};

#endif // CLOCKMASTERSYNCSEND_H
