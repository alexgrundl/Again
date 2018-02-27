#ifndef MDPDELAYREQ_H
#define MDPDELAYREQ_H

#include <cstdlib>

#include "statemachinebase.h"
#include "ptpmessage/ptpmessagepdelayreq.h"
#include "ptpmessage/ptpmessagepdelayresp.h"
#include "ptpmessage/ptpmessagepdelayrespfollowup.h"

class MDPdelayReq : public StateMachineBaseMD
{
public:

    MDPdelayReq(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGlobal,
                INetworkInterfacePort* networkPort);

    ~MDPdelayReq();

    /**
     * @brief Creates a structure containing the parameters of a Pdelay_Req message to be
     * transmitted, and returns a pointer, txPdelayReqPtr, to this structure.
     * @return A pointer to the created structure.
     */
    PtpMessagePDelayReq* SetPdelayReq();

    /**
     * @brief Transmits a Pdelay_Req message from the MD entity, containing the parameters in the structure pointed to by
     * txPdelayReqPtr.
     * @param txPdelayReqPtr The message to transmit.
     */
    void TxPdelayReq(PtpMessagePDelayReq* txPdelayReqPtr);

    /**
     * @brief Computes neighborRateRatio.
     */
    double ComputePdelayRateRatio();

    /**
     * @brief Computes the mean propagation delay on the link attached to this MD entity.
     */
    UScaledNs ComputePropTime();


    void ProcessState();


    void SetPDelayResponse(IReceivePackage *package);


    void SetPDelayResponseFollowUp(IReceivePackage *package);

private:

    /**
     * @brief A variable used to save the time at which the Pdelay_Req interval timer is started. A Pdelay_Req message
     * is sent when this timer expires.
     */
    UScaledNs m_pdelayIntervalTimer;

    /**
     * @brief A Boolean variable that notifies the current state machine when a
    Pdelay_Resp message is received. This variable is reset by the current state machine.
     */
    bool m_rcvdPdelayResp;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of the Pdelay_Resp
     * message whose receipt is indicated by rcvdPdelayResp (see 11.2.15.1.2).
     */
    PtpMessagePDelayResp* m_rcvdPdelayRespPtr;

    /**
     * @brief A Boolean variable that notifies the current state machine when a Pdelay_Resp_Follow_Up message is received.
     * This variable is reset by the current state machine.
     */
    bool m_rcvdPdelayRespFollowUp;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of the Pdelay_Resp_Follow_Up message
     * whose receipt is indicated by rcvdPdelayRespFollowUp.
     */
    PtpMessagePDelayRespFollowUp* m_rcvdPdelayRespFollowUpPtr;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of a Pdelay_Req message to be transmitted.
     */
    PtpMessagePDelayReq* m_txPdelayReqPtr;

    /**
     * @brief A Boolean variable that notifies the current state machine when the <pdelayReqEventEgressTimestamp> for a
     * transmitted Pdelay_Req message is received. This variable is reset by the current state machine.
     */
    bool m_rcvdMDTimestampReceive;

    /**
     * @brief A variable that holds the sequenceId for the next Pdelay_Req message to be transmitted by this MD entity.
     * The data type for pdelayReqSequenceId is UInteger16.
     */
    uint16_t m_pdelayReqSequenceId;

    /**
     * @brief A Boolean variable that indicates whether or not initial Pdelay_Resp and Pdelay_Resp_Follow_Up messages have been
     * received when initializing the neighborRateRatio measurement. This variable is initialized to FALSE when the port initializes
     * or re-initializes, and after a Pdelay_Resp and/or Pdelay_Resp_Follow_Up message are not received in response to a sent
     * Pdelay_Req message.
     */
    bool m_initPdelayRespReceived;

    /**
     * @brief A count of the number of consecutive Pdelay_Req messages sent by the port, for which Pdelay_Resp and/or
     * Pdelay_Resp_Follow_Up messages are not received.
     */
    uint16_t m_lostResponses;

    /**
     * @brief A Boolean variable that indicates whether or not the function computePdelayRateRatio() successfully
     * computed neighborRateRatio.
     */
    bool m_neighborRateRatioValid;


    UScaledNs m_pdelayRespTime;


    UScaledNs m_pdelayRespFollowUpCorrTime;

    void ExecuteResetState();


    void ExecuteSendPDelayReqState();


//    void NS_ReceiveMessage(bool followUp);
};

#endif // MDPDELAYREQ_H
