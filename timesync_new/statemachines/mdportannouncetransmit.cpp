#include "mdportannouncetransmit.h"

MDPortAnnounceTransmit::MDPortAnnounceTransmit(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetPort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_announceReceived = false;
    m_txAnnounceMessage = new PtpMessageAnnounce();
}

MDPortAnnounceTransmit::~MDPortAnnounceTransmit()
{
    delete m_txAnnounceMessage;
}

void MDPortAnnounceTransmit::SetAnnounce(PtpMessageAnnounce* announceMessage)
{
    uint8_t gmIdentity[CLOCK_ID_LENGTH];
    PortIdentity sourcePortIdentity = announceMessage->GetSourcePortIdentity();

    m_txAnnounceMessage->SetDomainNumber(announceMessage->GetDomainNumber());
    m_txAnnounceMessage->SetCorrectionField(announceMessage->GetCorrectionField());
    m_txAnnounceMessage->SetSourcePortIdentity(&sourcePortIdentity);
    m_txAnnounceMessage->SetSequenceID(announceMessage->GetSequenceID());
    m_txAnnounceMessage->SetLogMessageInterval(announceMessage->GetLogMessageInterval());

    m_txAnnounceMessage->SetCurrentUtcOffset(announceMessage->GetCurrentUtcOffset());
    m_txAnnounceMessage->SetGrandmasterPriority1(announceMessage->GetGrandmasterPriority1());
    m_txAnnounceMessage->SetGrandmasterClockQuality(announceMessage->GetGrandmasterClockQuality());
    m_txAnnounceMessage->SetGrandmasterPriority2(announceMessage->GetGrandmasterPriority2());
    announceMessage->GetGrandmasterIdentity(gmIdentity);
    m_txAnnounceMessage->SetGrandmasterIdentity(gmIdentity);

    m_txAnnounceMessage->SetStepsRemoved(announceMessage->GetStepsRemoved());
    m_txAnnounceMessage->SetTimeSource(announceMessage->GetTimeSource());
    m_txAnnounceMessage->SetPathSequence(announceMessage->GetPathSequence());

    m_txAnnounceMessage->SetFlagLeap61(announceMessage->GetFlagLeap61());
    m_txAnnounceMessage->SetFlagLeap59(announceMessage->GetFlagLeap59());
    m_txAnnounceMessage->SetCurrentUtcOffsetValid(announceMessage->GetCurrentUtcOffsetValid());
    m_txAnnounceMessage->SetTimeTraceable(announceMessage->GetTimeTraceable());
    m_txAnnounceMessage->SetFrequencyTraceable(announceMessage->GetFrequencyTraceable());

    m_announceReceived = true;
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
            {
                m_announceReceived = false;
                m_networkPort->SendGenericMessage(m_txAnnounceMessage);
                m_state = STATE_TRANSMIT_ANNOUNCE;
            }
            break;

        case STATE_TRANSMIT_ANNOUNCE:
            if(m_announceReceived)
            {
                m_announceReceived = false;
                m_networkPort->SendGenericMessage(m_txAnnounceMessage);
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
