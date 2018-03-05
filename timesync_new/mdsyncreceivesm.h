#ifndef MDSYNCRECEIVESM_H
#define MDSYNCRECEIVESM_H

#include "statemachinebase.h"
#include "portsyncsyncreceive.h"
#include "ptpmessage/ptpmessagesync.h"
#include "ptpmessage/ptpmessagefollowup.h"

class MDSyncReceiveSM : public StateMachineBaseMD
{
public:


    MDSyncReceiveSM(TimeAwareSystem* timeAwareSystem, PortGlobal* port, std::shared_ptr<PortSyncSyncReceive> portSyncSyncReceive,
                    INetworkInterfacePort *network);


    virtual ~MDSyncReceiveSM();


    void ProcessState();


    void SetSyncMessage(IReceivePackage* package);


    void SetFollowUpMessage(IReceivePackage *package);

private:

    /**
     * @brief A variable used to save the time at which the information conveyed by a received Sync message will be discarded if
     * the associated Follow_Up message is not received by then.
     */
    UScaledNs m_followUpReceiptTimeoutTime;

    /**
     * @brief A Boolean variable that notifies the current state machine when a Sync message is received.
     * This variable is reset by the current state machine.
     */
    bool m_rcvdSync;

    /**
     * @brief A Boolean variable that notifies the current state machine when a Follow_Up message is received. This variable is
     * reset by the current state machine.
     */
    bool m_rcvdFollowUp;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of the Sync message whose receipt is indicated by rcvdSync.
     */
    std::unique_ptr<PtpMessageSync> m_rcvdSyncPtr;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of the Follow_Up message whose receipt is indicated by rcvdFollowUp.
     */
    std::unique_ptr<PtpMessageFollowUp> m_rcvdFollowUpPtr;

    /**
     * @brief A pointer to a structure whose members contain the values of the parameters of an MDSyncReceive structure to be transmitted.
     */
    std::unique_ptr<MDSyncReceive> m_txMDSyncReceivePtr;

    /**
     * @brief The sync interval for the upstream port that sent the received Sync message.
     */
    UScaledNs m_upstreamSyncInterval;



    std::shared_ptr<PortSyncSyncReceive> m_portSyncSyncReceive;

    /**
     * @brief Creates an MDSyncReceive structure, and returns a pointer to this structure. The members of this structure are set as follows
     * @return The created MDSyncReceive structure.
     */
    void SetMDSyncReceive();

    /**
     * @brief Transmits an MDSyncReceive structure to the PortSyncSyncReceive state machine of the PortSync entity of this port.
     * @param txMDSyncReceivePtr The structure to transmit.
     */
    void TxMDSyncReceive();
};

#endif // MDSYNCRECEIVESM_H
