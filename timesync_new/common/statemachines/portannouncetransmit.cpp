#include "portannouncetransmit.h"

PortAnnounceTransmit::PortAnnounceTransmit(TimeAwareSystem *timeAwareSystem, SystemPort *port, MDPortAnnounceTransmit* portAnnounceTransmit) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_mdPortAnnounceTransmit = portAnnounceTransmit;
    m_txAnnounceMessage = new PtpMessageAnnounce();
}

PortAnnounceTransmit::~PortAnnounceTransmit()
{
    delete m_txAnnounceMessage;
}

void PortAnnounceTransmit::TxAnnounce()
{
    PortIdentity portIdentity;

    memcpy(portIdentity.clockIdentity, m_systemPort->GetIdentity().clockIdentity, CLOCK_ID_LENGTH);
    portIdentity.portNumber = m_systemPort->GetIdentity().portNumber;
    m_txAnnounceMessage->SetSourcePortIdentity(&portIdentity);

    m_txAnnounceMessage->SetLogMessageInterval(m_systemPort->GetCurrentLogAnnounceInterval());
    m_txAnnounceMessage->SetCurrentUtcOffset(m_timeAwareSystem->GetCurrentUtcOffset());
    m_txAnnounceMessage->SetGrandmasterPriority1(m_systemPort->GetMasterPriority().identity.priority1);
    m_txAnnounceMessage->SetGrandmasterClockQuality(m_systemPort->GetMasterPriority().identity.clockQuality);
    m_txAnnounceMessage->SetGrandmasterPriority2(m_systemPort->GetMasterPriority().identity.priority2);
    m_txAnnounceMessage->SetGrandmasterIdentity(m_systemPort->GetMasterPriority().identity.clockIdentity);

    m_txAnnounceMessage->SetStepsRemoved(m_timeAwareSystem->GetMasterStepsRemoved());

    m_txAnnounceMessage->SetFlagLeap61(m_timeAwareSystem->GetLeap61());
    m_txAnnounceMessage->SetFlagLeap59(m_timeAwareSystem->GetLeap59());
    m_txAnnounceMessage->SetCurrentUtcOffsetValid(m_timeAwareSystem->GetCurrentUtcOffsetValid());
    m_txAnnounceMessage->SetTimeTraceable(m_timeAwareSystem->GetTimeTraceable());
    m_txAnnounceMessage->SetFrequencyTraceable(m_timeAwareSystem->GetFrequencyTraceable());
    m_txAnnounceMessage->SetCurrentUtcOffset(m_timeAwareSystem->GetCurrentUtcOffset());
    m_txAnnounceMessage->SetTimeSource(m_timeAwareSystem->GetTimeSource());

    m_txAnnounceMessage->SetSequenceID(m_sequenceID);

    m_txAnnounceMessage->SetPathSequence(m_timeAwareSystem->GetPathTrace());

    m_txAnnounceMessage->SetDomainNumber(m_timeAwareSystem->GetDomain());

    m_mdPortAnnounceTransmit->SetAnnounce(m_txAnnounceMessage);
}

void PortAnnounceTransmit::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_systemPort->SetNewInfo(true);
        m_state = STATE_TRANSMIT_INIT;
    }
    else
    {
        switch (m_state)
        {
        case STATE_TRANSMIT_INIT:
            m_sequenceID = rand() % 65536;
            ExecuteIdleState();
            m_state = STATE_IDLE;
            break;
        case STATE_TRANSMIT_ANNOUNCE:
        case STATE_TRANSMIT_PERIODIC:
            ExecuteIdleState();
            m_state = STATE_IDLE;
            break;

        case STATE_IDLE:
            if(m_timeAwareSystem->ReadCurrentTime() >= m_announceSendTime && m_systemPort->IsSelected() && !m_systemPort->GetUpdtInfo())
            {
                m_systemPort->SetNewInfo(m_systemPort->GetNewInfo() || m_timeAwareSystem->GetSelectedRole(m_systemPort->GetIdentity().portNumber) == PORT_ROLE_MASTER);
                m_state = STATE_TRANSMIT_PERIODIC;
            }
            else if(m_systemPort->GetNewInfo() && (m_timeAwareSystem->GetSelectedRole(m_systemPort->GetIdentity().portNumber) == PORT_ROLE_MASTER) &&
                    (m_timeAwareSystem->ReadCurrentTime() < m_announceSendTime) && m_systemPort->IsSelected() && !m_systemPort->GetUpdtInfo())
            {
                m_systemPort->SetNewInfo(false);
                //Only send the announce message if our system is grandmaster - capable (priority1 < 255).
                if(m_timeAwareSystem->IsGmPresent())
                {
                    TxAnnounce();
                    m_sequenceID++;
                }
                m_state = STATE_TRANSMIT_ANNOUNCE;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void PortAnnounceTransmit::ExecuteIdleState()
{
    m_announceSendTime = m_timeAwareSystem->ReadCurrentTime() + m_systemPort->GetAnnounceInterval();
}
