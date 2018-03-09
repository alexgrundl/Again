#ifndef MDPORTANNOUNCETRANSMIT_H
#define MDPORTANNOUNCETRANSMIT_H

#include <memory>

#include "statemachinebase.h"
#include "ptpmessage/ptpmessageannounce.h"

class MDPortAnnounceTransmit : public StateMachineBaseMD
{
public:


    MDPortAnnounceTransmit(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort);


    virtual ~MDPortAnnounceTransmit();


    void SetAnnounce(PtpMessageAnnounce* announceMessage);


    void ProcessState();

private:

    bool m_announceReceived;

    PtpMessageAnnounce* m_txAnnounceMessage;
};

#endif // MDPORTANNOUNCETRANSMIT_H
