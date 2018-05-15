#ifndef STATEMACHINEBASE_H
#define STATEMACHINEBASE_H

#include <vector>
#include <string.h>
#include <stdio.h>

#include "commstructs.h"
#include "timeawaresystem.h"
#include "systemport.h"

#include "inetport.h"
#include "platform.h"

class StateMachineBase
{
public:

    enum State
    {
        STATE_INITIALIZING = 0,
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

        STATE_RECEIVE = 25,
        STATE_DISABLED = 26,
        STATE_AGED = 27,
        STATE_UPDATE = 28,
        STATE_CURRENT = 29,
        STATE_SUPERIOR_MASTER_PORT = 30,
        STATE_REPEATED_MASTER_PORT = 31,
        STATE_INFERIOR_MASTER_OR_OTHER_PORT,

        STATE_INIT_BRIDGE,
        STATE_ROLE_SELECTION,

        STATE_TRANSMIT_ANNOUNCE,
        STATE_TRANSMIT_PERIODIC,
        STATE_IDLE,

        STATE_SET_INTERVALS,
        STATE_PLATFORM_SYNC
    };


    StateMachineBase(TimeAwareSystem* timeAwareSystem);


    virtual ~StateMachineBase();


    TimeAwareSystem* GetTimeAwareSystem();


    void SetTimeAwareSystem(TimeAwareSystem* timeAwareSystem);


    virtual void ProcessState() = 0;


protected:


    TimeAwareSystem* m_timeAwareSystem;


    State m_state;


    pal::SectionLock_t m_lock;
};


class StateMachineBasePort : public StateMachineBase
{

public:

    StateMachineBasePort(TimeAwareSystem* timeAwareSystem, SystemPort* portGlobal);


    virtual ~StateMachineBasePort();


protected:

    SystemPort* m_systemPort;
};



class StateMachineBaseMD : public StateMachineBasePort
{

public:

    StateMachineBaseMD(TimeAwareSystem* timeAwareSystem, SystemPort* portGlobal, INetPort* network);


    virtual ~StateMachineBaseMD();


protected:


    INetPort* m_networkPort;
};

#endif // STATEMACHINEBASE_H
