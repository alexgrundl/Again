#ifndef MDPORTANNOUNCETRANSMIT_H
#define MDPORTANNOUNCETRANSMIT_H

#include <memory>

#include "statemachinebase.h"
#include "ptpmessageannounce.h"

class MDPortAnnounceTransmit : public StateMachineBaseMD
{
public:


    MDPortAnnounceTransmit(TimeAwareSystem* timeAwareSystem, SystemPort* port, INetPort* networkPort);


    virtual ~MDPortAnnounceTransmit();


    void SetAnnounce(PtpMessageAnnounce* announceMessage);


    void ProcessState();

private:

    bool m_announceReceived;

    PtpMessageAnnounce* m_txAnnounceMessage;
};

#endif // MDPORTANNOUNCETRANSMIT_H
