#ifndef LINKDELAYSYNCINTERVALSETTING_H
#define LINKDELAYSYNCINTERVALSETTING_H

#include "statemachinebase.h"
#include "ptpmessagesignaling.h"

class LinkDelaySyncIntervalSetting : public StateMachineBaseMD
{
public:


    LinkDelaySyncIntervalSetting(TimeAwareSystem* timeAwareSystem, SystemPort* port, INetPort* networkPort);


    ~LinkDelaySyncIntervalSetting();


    void SetSignalingMessage(ReceivePackage *package);

private:

    void ProcessState();

    /**
     * @brief a Boolean variable that notifies the current state machine when a Signaling message that contains a Message Interval
     * Request TLV (see 10.5.4.3) is received. This variable is reset by the current state machine.
     */
    bool m_rcvdSignalingMsg1;

    /**
     * @brief A pointer to a structure whose members contain the values of the fields of the received Signaling message that contains
     * a Message Interval Request TLV.
     */
    PtpMessageSignaling* m_rcvdSignalingPtr;
};

#endif // LINKDELAYSYNCINTERVALSETTING_H
