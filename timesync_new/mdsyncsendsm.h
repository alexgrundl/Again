#ifndef MDSYNCSENDSM_H
#define MDSYNCSENDSM_H

#include <cstdlib>

#include "statemachinebase.h"
#include "ptpmessage/ptpmessagesync.h"
#include "ptpmessage/ptpmessagefollowup.h"

class MDSyncSendSM : public StateMachineBaseMD
{
public:

    MDSyncSendSM(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGloball);

    virtual ~MDSyncSendSM();

    /**
     * @brief Creates a structure whose parameters contain the fields (see 11.4 and its subclauses) of a Sync message to be transmitted,
     * and returns a pointer to this structure. The parameters are set as follows.
     * @return A pointer to the created sync message.
     */
    PtpMessageSync* SetSync();

    /**
     * @brief Transmits a Sync message from this MD entity, whose fields contain the parameters in the structure pointed to by txSyncPtr.
     * @param txSyncPtr Pointer to the message to transmit.
     */
    void TxSync(PtpMessageSync* txSyncPtr);

    /**
     * @brief Creates a structure whose parameters contain the fields of a Follow_Up message to be transmitted, and returns a pointer to this structure.
     * @return A pointer to the created follow up message.
     */
    PtpMessageFollowUp* SetFollowUp();

    /**
     * @brief Transmits a Follow_Up message from this MD entity, whose fields contain the parameters in the structure pointed to by txFollowUpPtr.
     * @param txFollowUpPtr Pointer to the message to transmit.
     */
    void TxFollowUp(PtpMessageFollowUp* txFollowUpPtr);


    void ProcessState();


    void SetMDSyncSend(MDSyncSend* rcvdMDSyncPtr);


    void SetMDTimestampReceive(MDTimestampReceive* rcvdMDTimestampReceivePtr);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when an MDSyncSend structure is received.
     * This variable is reset by the current state machine.
     */
    bool m_rcvdMDSync;
    /**
     * @brief A pointer to a structure whose members contain the values of the fields of a Sync message to be transmitted.
     */
    PtpMessageSync* m_txSyncPtr;
    /**
     * @brief A Boolean variable that notifies the current state machine when the <syncEventEgressTimestamp> for a transmitted
     * Sync message is received. This variable is reset by the current state machine.
     */
    bool m_rcvdMDTimestampReceive;

    /**
     * @brief A pointer to the received MDTimestampReceive structure.
     */
    MDTimestampReceive* m_rcvdMDTimestampReceivePtr;
    /**
     * @brief A pointer to a structure whose members contain the values of the fields of a Follow_Up message to be transmitted.
     */
    PtpMessageFollowUp* m_txFollowUpPtr;



    /**
     * @brief The MDSyncSend structure received from the PortSyncSyncSend state machine.
     */
    MDSyncSend* m_rcvdMDSyncPtr;
};

#endif // MDSYNCSENDSM_H
