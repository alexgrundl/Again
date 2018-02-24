#ifndef STATEMACHINEBASE_H
#define STATEMACHINEBASE_H

#include <vector>
#include <string.h>
#include <stdio.h>

#include "commstructs.h"

class StateMachineBase
{
public:

    enum State
    {
        STATE_INITIALIZING,
        STATE_RECEIVING_SYNC,
        STATE_DISCARD,
        STATE_RECEIVED_SYNC,
        STATE_SEND_SYNC_INDICATION,
        STATE_WAITING,
        STATE_RECEIVE_SOURCE_TIME,
        STATE_TRANSMIT_INIT,
        STATE_SEND_MD_SYNC,
        STATE_SET_SYNC_RECEIPT_TIMEOUT_TIME,
        STATE_SYNC_RECEIPT_TIMEOUT,
        STATE_WAITING_FOR_FOLLOW_UP,
        STATE_WAITING_FOR_SYNC,
        STATE_SEND_SYNC,
        STATE_SEND_FOLLOW_UP,

        STATE_NOT_ENABLED,
        STATE_INITIAL_SEND_PDELAY_REQ,
        STATE_SEND_PDELAY_REQ,
        STATE_WAITING_FOR_PDELAY_RESP,
        STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP,
        STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER,
        STATE_RESET,

        STATE_INITIAL_WAITING_FOR_PDELAY_REQ,
        STATE_WAITING_FOR_PDELAY_REQ,
        STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP
    };


    StateMachineBase(TimeAwareSystem* timeAwareSystem);


    virtual ~StateMachineBase();


    TimeAwareSystem* GetTimeAwareSystem();


    void SetTimeAwareSystem(TimeAwareSystem* timeAwareSystem);


    virtual void ProcessState() = 0;


protected:


    TimeAwareSystem* m_timeAwareSystem;


    State m_state;
};


class StateMachineBasePort : public StateMachineBase
{

public:

    StateMachineBasePort(TimeAwareSystem* timeAwareSystem, PortGlobal* portGlobal);


    virtual ~StateMachineBasePort();


    void SetMDGlobal(MDGlobal* mdGlobal);


protected:

    PortGlobal* m_portGlobal;
};



class StateMachineBaseMD : public StateMachineBasePort
{

public:

    StateMachineBaseMD(TimeAwareSystem* timeAwareSystem, PortGlobal* portGlobal, MDGlobal* mdGlobal);


    virtual ~StateMachineBaseMD();


    void SetMDGlobal(MDGlobal* mdGlobal);


protected:


    MDGlobal* m_mdGlobal;
};

#endif // STATEMACHINEBASE_H
