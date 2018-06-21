#include "platformsync.h"

PlatformSync::PlatformSync(TimeAwareSystem *timeAwareSystem, SystemPort *port, INetPort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_ptpClock = networkPort->GetPtpClock();
    m_timeControl.SetPtpClock(m_ptpClock);

    m_tsExtEvent = {0, 0};
    m_tsSystem = {0, 0};
    m_syntonize = true;
    m_externalTimestampRead = false;

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
            m_ptpClock->EnableExternalTimestamp();
            m_portThread->Start();
            m_state = STATE_PLATFORM_SYNC;
            break;

        case STATE_PLATFORM_SYNC:
            if(m_externalTimestampRead)
            {
                if(m_syntonize)
                {
                    if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
                    {
                        m_networkPort->GetPtpClock()->SetPtssOffset(m_timeControl.Syntonize(m_timeAwareSystem->GetLocalClock(), m_tsExtEvent, m_tsSystem, 0.125));
//                        if(m_networkPort->GetInterfaceName() == "enp3s0f0")
//                            printf("Port %s - offset: %li ns\n", m_networkPort->GetInterfaceName().c_str(), m_networkPort->GetPtpClock()->GetPtssOffset());
                    }
                }

                m_syntonize = !m_syntonize || m_networkPort->GetNetworkCardType() == NETWORK_CARD_TYPE_X540;

                m_externalTimestampRead = false;
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
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;

    /* Empty the internal kernel queue with the buffered timestamps. */
    while(m_ptpClock->ReadExternalTimestamp(m_tsExtEvent, m_tsSystem, timeout));

    timeout.tv_usec = 500000;
    //m_portThread->ChangePriority(PrioTimeCritical);
    while(*pbIsRunning)
    {
        if(m_ptpClock->ReadExternalTimestamp(m_tsExtEvent, m_tsSystem, timeout))
        {
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
                m_externalTimestampRead = true;
            else
            {
                /* The PPS synchronization of the main i210 has to be executed in a separate thread as
                 * some waiting is done, here (about 100 ms max.). Should really be made in another way as this
                   violates the logical constitution of the other state machines...*/
                /* The idea has to be that this sync should be handled in the "ProcessState" method. Here, we'd
                 * only inform this state machine that a new GPS PPS signal has arrived. Handling should be done there, then. */
                uint64_t deviceTime = m_tsExtEvent.tv_sec * NS_PER_SEC + m_tsExtEvent.tv_nsec;
                uint64_t systemTime = m_tsSystem.tv_sec * NS_PER_SEC + m_tsSystem.tv_nsec;
                m_timeAwareSystem->UpdateGPSDataFromPPS(deviceTime, systemTime);
            }

//            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_ROOT)
//                printf("%s: tsExtEvent.tv_sec: %lu, tsExtEvent.tv_nsec: %lu\n", m_networkPort->GetInterfaceName().c_str(), m_tsExtEvent.tv_sec, m_tsExtEvent.tv_nsec);
        }
    }

    return 0;
}
