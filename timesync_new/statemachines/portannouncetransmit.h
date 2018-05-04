#ifndef PORTANNOUNCETRANSMIT_H
#define PORTANNOUNCETRANSMIT_H

#include <memory>

#include "statemachinebase.h"
#include "mdportannouncetransmit.h"

class PortAnnounceTransmit : public StateMachineBasePort
{
public:


    PortAnnounceTransmit(TimeAwareSystem* timeAwareSystem, SystemPort* port, MDPortAnnounceTransmit* portAnnounceTransmit);


    virtual ~PortAnnounceTransmit();


    void ProcessState();

private:

    /**
     * @brief The time, relative to the LocalClock, at which the next transmission of Announce information is to occur.
     */
    UScaledNs m_announceSendTime;


    PtpMessageAnnounce* m_txAnnounceMessage;


    MDPortAnnounceTransmit* m_mdPortAnnounceTransmit;


    uint16_t m_sequenceID;


    /**
     * @brief Transmits Announce information to the MD entity of this port.
     */
    void TxAnnounce();


    void ExecuteIdleState();
};

#endif // PORTANNOUNCETRANSMIT_H
