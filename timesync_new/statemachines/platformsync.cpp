#include "platformsync.h"

PlatformSync::PlatformSync(TimeAwareSystem *timeAwareSystem, SystemPort *port, INetPort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_ptpClock = networkPort->GetPtpClock();
    m_timeControl.SetPtpClock(m_ptpClock);
    m_portThread = new CThreadWrapper<PlatformSync>(this, &PlatformSync::ReadTimestamp, std::string("PTSS thread Port ") +
                                                   std::to_string(networkPort->GetPortNumber()));
}

PlatformSync::~PlatformSync()
{
    m_portThread->Stop();
    delete m_portThread;
}

void PlatformSync::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
    }
    else
    {
        switch(m_state)
        {
        case STATE_INITIALIZING:
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
            {
                m_ptpClock->EnableExternalTimestamp(1);
                m_portThread->Start();
                m_state = STATE_PLATFORM_SYNC;
            }
            break;

        case STATE_PLATFORM_SYNC:
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
            {

            }
            else
            {
                m_portThread->Stop();
                m_ptpClock->DisableExternalTimestamp(1);
                m_state = STATE_INITIALIZING;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

uint32_t PlatformSync::ReadTimestamp(bool_t* pbIsRunning, pal::EventHandle_t /*pWaitHandle*/)
{
    bool syntonize = true;

    //m_portThread->ChangePriority(PrioTimeCritical);
    while(*pbIsRunning)
    {
        struct timespec tsExtEvent, tsSystem;
        if(m_ptpClock->ReadExternalTimestamp(tsExtEvent, tsSystem))
        {
            if(syntonize)
            {
                int64_t nsOffset = m_timeControl.Syntonize(m_timeAwareSystem->GetLocalClock(), tsExtEvent, tsSystem);
                printf("Port %u - offset: %li ns\n", m_networkPort->GetPortNumber(), nsOffset);
            }
            syntonize = !syntonize || m_networkPort->GetNetworkCardType() == NETWORK_CARD_TYPE_X540;
        }
    }

    return 0;
}
