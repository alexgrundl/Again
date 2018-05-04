#include "linkdelaysyncintervalsetting.h"

LinkDelaySyncIntervalSetting::LinkDelaySyncIntervalSetting(TimeAwareSystem *timeAwareSystem, SystemPort *port, INetPort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_rcvdSignalingMsg1 = false;
    m_rcvdSignalingPtr = new PtpMessageSignaling();

    m_state = STATE_NOT_ENABLED;
}

LinkDelaySyncIntervalSetting::~LinkDelaySyncIntervalSetting()
{
    delete m_rcvdSignalingPtr;
}

void LinkDelaySyncIntervalSetting::SetSignalingMessage(ReceivePackage *package)
{
    m_rcvdSignalingPtr->ParsePackage(package->GetBuffer());
    m_rcvdSignalingPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdSignalingMsg1 = true;
}

void LinkDelaySyncIntervalSetting::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || !m_systemPort->IsPortEnabled() || !m_systemPort->IsPttPortEnabled())
    {
        m_state = STATE_NOT_ENABLED;
    }
    else
    {
        switch(m_state)
        {
        case STATE_NOT_ENABLED:
            if(m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled())
            {
                m_systemPort->SetCurrentLogPdelayReqInterval(m_systemPort->GetInitialLogPdelayReqInterval());
                m_systemPort->SetCurrentLogSyncInterval(m_systemPort->GetInitialLogSyncInterval());
                m_systemPort->SetPdelayReqInterval({(uint64_t)(NS_PER_SEC * pow(2, m_systemPort->GetInitialLogPdelayReqInterval())), 0});

                m_systemPort->SetSyncInterval({(uint64_t)(NS_PER_SEC * pow(2, m_systemPort->GetInitialLogSyncInterval())), 0});
                m_systemPort->SetComputeNeighborRateRatio(true);
                m_systemPort->SetComputeNeighborPropDelay(true);
                m_rcvdSignalingMsg1 = false;
                m_state = STATE_INITIALIZING;
            }
            break;
        case STATE_INITIALIZING:
        case STATE_SET_INTERVALS:
            if(m_rcvdSignalingMsg1)
            {
                switch (m_rcvdSignalingPtr->GetLinkDelayInterval())
                {
                case (-128): /* don’t change the interval */
                    break;
                case 126: /* set interval to initial value */
                    m_systemPort->SetCurrentLogPdelayReqInterval(m_systemPort->GetInitialLogPdelayReqInterval());
                    m_systemPort->SetPdelayReqInterval({(uint64_t)(NS_PER_SEC * pow(2, m_systemPort->GetInitialLogPdelayReqInterval())), 0});
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the Pdelay
                    * requester should stop sending for all practical purposes. */
                    m_systemPort->SetPdelayReqInterval({(uint64_t)(NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetLinkDelayInterval())), 0});
                    break;
                }

                switch (m_rcvdSignalingPtr->GetTimeSyncInterval())
                {
                case (-128): /* don’t change the interval */
                    break;
                case 126: /* set interval to initial value */
                    m_systemPort->SetCurrentLogSyncInterval(m_systemPort->GetInitialLogSyncInterval());
                    m_systemPort->SetSyncInterval({(uint64_t)(NS_PER_SEC * pow(2, m_systemPort->GetInitialLogSyncInterval())), 0});
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the sender
                    * should stop sending for all practical purposes. */
                    m_systemPort->SetSyncInterval({(uint64_t)(NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetTimeSyncInterval())), 0});
                    break;
                }

                switch (m_rcvdSignalingPtr->GetAnnounceInterval())
                {
                case (-128): /* don’t change the interval */
                    break;
                case 126: /* set interval to initial value */
                    m_systemPort->SetCurrentLogAnnounceInterval(m_systemPort->GetInitialLogAnnounceInterval());
                    m_systemPort->SetAnnounceInterval({(uint64_t)(NS_PER_SEC * pow(2, m_systemPort->GetInitialLogAnnounceInterval())), 0});
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the sender
                    * should stop sending for all practical purposes. */
                    m_systemPort->SetAnnounceInterval({(uint64_t)(NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetAnnounceInterval())), 0});
                    break;
                }

                m_systemPort->SetComputeNeighborRateRatio(m_rcvdSignalingPtr->GetComputeNeighborRateRatio());
                m_systemPort->SetComputeNeighborPropDelay(m_rcvdSignalingPtr->GetComputeNeighborPropDelay());

                m_rcvdSignalingMsg1 = false;
                m_state = STATE_SET_INTERVALS;
            }
            break;
        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
