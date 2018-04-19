#include "portannouncetransmit.h"

PortAnnounceTransmit::PortAnnounceTransmit(TimeAwareSystem *timeAwareSystem, PortGlobal *port, MDPortAnnounceTransmit* portAnnounceTransmit) :
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

    memcpy(portIdentity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH);
    portIdentity.portNumber = m_portGlobal->identity.portNumber;
    m_txAnnounceMessage->SetSourcePortIdentity(&portIdentity);

    m_txAnnounceMessage->SetLogMessageInterval(m_portGlobal->currentLogAnnounceInterval);
    m_txAnnounceMessage->SetCurrentUtcOffset(m_timeAwareSystem->currentUtcOffset);
    m_txAnnounceMessage->SetGrandmasterPriority1(m_portGlobal->masterPriority.identity.priority1);
    m_txAnnounceMessage->SetGrandmasterClockQuality(m_portGlobal->masterPriority.identity.clockQuality);
    m_txAnnounceMessage->SetGrandmasterPriority2(m_portGlobal->masterPriority.identity.priority2);
    m_txAnnounceMessage->SetGrandmasterIdentity(m_portGlobal->masterPriority.identity.clockIdentity);

    m_txAnnounceMessage->SetStepsRemoved(m_timeAwareSystem->masterStepsRemoved);

    m_txAnnounceMessage->SetFlagLeap61(m_timeAwareSystem->leap61);
    m_txAnnounceMessage->SetFlagLeap59(m_timeAwareSystem->leap59);
    m_txAnnounceMessage->SetCurrentUtcOffsetValid(m_timeAwareSystem->currentUtcOffsetValid);
    m_txAnnounceMessage->SetTimeTraceable(m_timeAwareSystem->timeTraceable);
    m_txAnnounceMessage->SetFrequencyTraceable(m_timeAwareSystem->frequencyTraceable);
    m_txAnnounceMessage->SetCurrentUtcOffset(m_timeAwareSystem->currentUtcOffset);
    m_txAnnounceMessage->SetTimeSource(m_timeAwareSystem->timeSource);

    m_txAnnounceMessage->SetSequenceID(m_sequenceID);

    m_txAnnounceMessage->SetPathSequence(m_timeAwareSystem->GetPathTrace());

    m_mdPortAnnounceTransmit->SetAnnounce(m_txAnnounceMessage);
}

void PortAnnounceTransmit::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_portGlobal->newInfo = true;
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
            if(m_timeAwareSystem->GetCurrentTime() >= m_announceSendTime && m_timeAwareSystem->selected[m_portGlobal->identity.portNumber - 1] &&
                    !m_portGlobal->updtInfo)
            {
                m_portGlobal->newInfo = m_portGlobal->newInfo || m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_MASTER;
                m_state = STATE_TRANSMIT_PERIODIC;
            }
            else if(m_portGlobal->newInfo && (m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_MASTER) &&
                    (m_timeAwareSystem->GetCurrentTime() < m_announceSendTime) && m_timeAwareSystem->selected[m_portGlobal->identity.portNumber - 1]
                    && !m_portGlobal->updtInfo)
            {
                m_portGlobal->newInfo = false;
                TxAnnounce();
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
    m_announceSendTime = m_timeAwareSystem->GetCurrentTime() + m_portGlobal->announceInterval;
}
