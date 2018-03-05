#include "mdportannouncetransmit.h"

MDPortAnnounceTransmit::MDPortAnnounceTransmit(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_announceReceived = false;
    m_txAnnounceMessage = std::unique_ptr<PtpMessageAnnounce>(new PtpMessageAnnounce());
}

void MDPortAnnounceTransmit::SetAnnounce(std::shared_ptr<PtpMessageAnnounce> announceMessage)
{
    m_txAnnounceMessage = announceMessage;
}

void MDPortAnnounceTransmit::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_announceReceived = false;
        m_state = STATE_TRANSMIT_INIT;
    }
    else
    {
        switch(m_state)
        {
        case STATE_TRANSMIT_INIT:
            if(m_announceReceived)
                m_networkPort->SendGenericMessage(m_txAnnounceMessage.get());
            m_state = STATE_TRANSMIT_ANNOUNCE;
            break;

        case STATE_TRANSMIT_ANNOUNCE:
            if(m_announceReceived)
                m_networkPort->SendGenericMessage(m_txAnnounceMessage.get());
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
