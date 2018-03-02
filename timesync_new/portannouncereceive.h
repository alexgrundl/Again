#ifndef PORTANNOUNCERECEIVE_H
#define PORTANNOUNCERECEIVE_H

#include "statemachinebase.h"
#include "ptpmessage/ptpmessageannounce.h"
#include "portannounceinformation.h"

class PortAnnounceReceive : public StateMachineBasePort
{
public:

    PortAnnounceReceive(TimeAwareSystem* timeAwareSystem, PortGlobal* port, PortAnnounceInformation* portAnnounceInformation);


    ~PortAnnounceReceive();


    void ProcessState();


    void SetAnnounce(IReceivePackage *package);

private:

    /**
     * @brief a Boolean variable that notifies the current state machine when Announce message information is received from the MD entity of the
     * same port. This variable is reset by this state machine.
     */
    bool m_rcvdAnnounce;


    PortAnnounceInformation* m_portAnnounceInformation;

    /**
     * @brief Qualifies the received Announce message pointed to by rcvdAnnouncePtr.
     * @param rcvdAnnouncePtr The message to qualify.
     * @return a) If the Announce message was sent by the current time-aware system, i.e., if sourcePortIdentity.clockIdentity (see 10.5.2.2.8 and 8.5.2) is
     * equal to thisClock (see 10.2.3.22), the Announce message is not qualified and FALSE is returned;
     * b) If the stepsRemoved field is greater than or equal to 255, the Announce message is not qualified and FALSE is returned;
     * c) If a path trace TLV is present and one of the elements of the pathSequence array field of the path trace TLV is equal to thisClock
     * (i.e., the clockIdentity of the current time-aware system, see 10.2.3.22), the Announce message is not qualified and FALSE is returned;
     * d) Otherwise, the Announce message is qualified and TRUE is returned. If a path trace TLV is present and the portRole of the port is SlavePort,
     * the pathSequence array field of the TLV is copied to the global array pathTrace, and thisClock is appended to pathTrace
     * (i.e., is added to the end of the array).
     */
    bool QualifyAnnounce(PtpMessageAnnounce* rcvdAnnouncePtr);
};

#endif // PORTANNOUNCERECEIVE_H
