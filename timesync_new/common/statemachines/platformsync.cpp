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
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
            {
                m_ptpClock->EnableExternalTimestamp();
                m_portThread->Start();
                m_state = STATE_PLATFORM_SYNC;
            }
            break;

        case STATE_PLATFORM_SYNC:
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_SLAVE)
            {
                if(m_externalTimestampRead)
                {
                    if(m_syntonize)
                    {
                        m_networkPort->GetPtpClock()->SetPtssOffset(m_timeControl.Syntonize(m_timeAwareSystem->GetLocalClock(), m_tsExtEvent, m_tsSystem));
                        //printf("Port %s - offset: %li ns\n", m_networkPort->GetInterfaceName().c_str(), nsOffset);
                    }
                    m_syntonize = !m_syntonize || m_networkPort->GetNetworkCardType() == NETWORK_CARD_TYPE_X540;

                    m_externalTimestampRead = false;
                }
            }
            else
            {
                m_portThread->Stop();
                m_ptpClock->DisableExternalTimestamp();
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
    //m_portThread->ChangePriority(PrioTimeCritical);
    while(*pbIsRunning)
    {
        if(m_ptpClock->ReadExternalTimestamp(m_tsExtEvent, m_tsSystem))
        {
            m_externalTimestampRead = true;
        }
    }

    return 0;
}
