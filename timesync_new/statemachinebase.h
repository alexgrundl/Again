#ifndef STATEMACHINEBASE_H
#define STATEMACHINEBASE_H

#include <vector>
#include <string.h>
#include <stdio.h>

#include "commstructs.h"
#include "timeawaresystem.h"

#include "comm/linux_netport.h"

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
        STATE_SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP,

        STATE_RECEIVE,
        STATE_DISABLED,
        STATE_AGED,
        STATE_UPDATE,
        STATE_CURRENT,
        STATE_SUPERIOR_MASTER_PORT,
        STATE_REPEATED_MASTER_PORT,
        STATE_INFERIOR_MASTER_OR_OTHER_PORT,

        STATE_INIT_BRIDGE,
        STATE_ROLE_SELECTION,

        STATE_TRANSMIT_ANNOUNCE,
        STATE_TRANSMIT_PERIODIC,
        STATE_IDLE
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


protected:

    PortGlobal* m_portGlobal;
};



class StateMachineBaseMD : public StateMachineBasePort
{

public:

    StateMachineBaseMD(TimeAwareSystem* timeAwareSystem, PortGlobal* portGlobal, INetworkInterfacePort* network);


    virtual ~StateMachineBaseMD();


protected:


    INetworkInterfacePort* m_networkPort;
};

#endif // STATEMACHINEBASE_H
