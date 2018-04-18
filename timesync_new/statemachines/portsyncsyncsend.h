#ifndef PORTSYNCSYNCSEND_H
#define PORTSYNCSYNCSEND_H

#include <memory>

#include "statemachinebase.h"
#include "mdsyncsendsm.h"

class PortSyncSyncSend : public StateMachineBasePort
{
public:

    PortSyncSyncSend(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDSyncSendSM* mdSyncSendSM);


    virtual ~PortSyncSyncSend();


    void ProcessSync(PortSyncSync* rcvd);


    void ProcessState();

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when a PortSyncSync structure is received from
     * the SiteSyncSync state machine of the SiteSync entity of the time-aware system. This variable is reset by this state machine.
     */
    bool m_rcvdPSSync;

    /**
     * @brief A pointer to the received PortSyncSync structure indicated by rcvdPSSync.
     */
    PortSyncSync* m_rcvdPSSyncPtr;

    /**
     * @brief The sourcePortIdentity member of the most recently received PortSyncSync structure.
     */
    PortIdentity m_lastSourcePortIdentity;

    /**
     * @brief The preciseOriginTimestamp member of the most recently received PortSyncSync structure.
     */
    Timestamp m_lastPreciseOriginTimestamp;

    /**
     * @brief The followUpCorrectionField member of the most recently received PortSyncSync member.
     */
    ScaledNs m_lastFollowUpCorrectionField;

    /**
     * @brief The rateRatio member of the most recently received PortSyncSync structure.
     */
    double m_lastRateRatio;

    /**
     * @brief The upstreamTxTime of the most recently received PortSyncSync member.
     */
    UScaledNs m_lastUpstreamTxTime;

    /**
     * @brief The value of currentTime (i.e., the time relative to the LocalClock entity) when the most recent MDSyncSend
     * structure was sent.
     */
    UScaledNs m_lastSyncSentTime;

    /**
     * @brief The portNumber of the port on which time-synchronization information was most recently received.
     */
    uint16_t m_lastRcvdPortNum;

    /**
     * @brief The gmTimeBaseIndicator of the most recently received PortSyncSync member.
     */
    uint16_t m_lastGmTimeBaseIndicator;

    /**
     * @brief The lastGmPhaseChange of the most recently received PortSyncSync member.
     */
    ScaledNs m_lastGmPhaseChange;

    /**
     * @brief The lastGmFreqChange of the most recently received PortSyncSync member.
     */
    double m_lastGmFreqChange;

    /**
     * @brief  A pointer to the MDSyncSend structure sent to the MD entity of this port.
     */
    MDSyncSend* m_txMDSyncSendPtr;

    /**
     * @brief The value of the syncReceiptTimeoutTime member of the most recently received PortSyncSync structure.
     */
    UScaledNs m_syncReceiptTimeoutTime;



    MDSyncSendSM* m_mdSyncSendSM;


    /**
     * @brief Creates an MDSyncSend structure, and returns a pointer to this structure.
     * @return A pointer to the created structure.
     */
    void SetMDSync();

    /**
     * @brief Transmits the MDSyncSend structure pointed to by txMDSyncSendPtr, to the MDSyncSendSM state machine of the MD entity of this port.
     * @param txMDSyncPtr The structure to transmit.
     */
    void TxMDSync();


    void ExecuteSendMDSyncState();

};

#endif // PORTSYNCSYNCSEND_H
