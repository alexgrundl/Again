#ifndef MDPDELAYRESP_H
#define MDPDELAYRESP_H

#include <memory>

#include "statemachinebase.h"
#include "ptpmessagepdelayresp.h"
#include "ptpmessagepdelayrespfollowup.h"
#include "ptpmessagepdelayreq.h"

class MDPdelayResp : public StateMachineBaseMD
{
public:


    MDPdelayResp(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort *networkPort);


    virtual ~MDPdelayResp();


    void ProcessState();


    void SetPDelayRequest(IReceivePackage* package);

private:

    /**
     * @brief A Boolean variable that notifies the current state machine when a Pdelay_Req message is received.
     * This variable is reset by the current state machine.
     */
    bool m_rcvdPdelayReq;

    /**
     * @brief A Boolean variable that notifies the current state machine when the <pdelayRespEventEgressTimestamp>
     * for a transmitted Pdelay_Resp message is received. This variable is reset by the current state machine.
     */
    bool m_rcvdMDTimestampReceive;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of a Pdelay_Resp message to be
     * transmitted.
     */
    PtpMessagePDelayResp* m_txPdelayRespPtr;

    /**
     * @brief  Apointer to a structure whose members contain the values of the fields of a Pdelay_Resp_Follow_Up message
     * to be transmitted.
     */
    PtpMessagePDelayRespFollowUp* m_txPdelayRespFollowUpPtr;


    PtpMessagePDelayReq* m_rcvdPdelayReqPtr;


    /**
     * @brief Creates a structure containing the parameters (see 11.4 and its subclauses) of a Pdelay_Resp message
     * to be transmitted, and returns a pointer, txPdelayRespPtr, to this structure.
     * @return The created structure.
     */
    void SetPdelayResp();

    /**
     * @brief Transmits a Pdelay_Resp message from the MD entity, containing the parameters in the structure pointed
     * to by txPdelayRespPtr.
     * @param txPdelayRespPtr The structure to transmit.
     */
    void TxPdelayResp();

    /**
     * @brief Creates a structure containing the parameters of a Pdelay_Resp_Follow_Up message  to be transmitted,
     * and returns a pointer, txPdelayRespFollowUpPtr, to this structure.
     * @return The created structure.
     */
    void SetPdelayRespFollowUp();

    /**
     * @brief Transmits a Pdelay_Resp_Follow_Up message from the P2PPort entity containing the parameters in the structure
     * pointed to by txPdelayRespFollowUpPtr.
     * @param txFollowUpPtr The structure to transmit.
     */
    void TxPdelayRespFollowUp();
};

#endif // MDPDELAYRESP_H
