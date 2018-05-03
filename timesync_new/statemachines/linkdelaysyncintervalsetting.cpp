#include "linkdelaysyncintervalsetting.h"

LinkDelaySyncIntervalSetting::LinkDelaySyncIntervalSetting(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort *networkPort) :
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

void LinkDelaySyncIntervalSetting::SetSignalingMessage(IReceivePackage *package)
{
    m_rcvdSignalingPtr->ParsePackage(package->GetBuffer());
    m_rcvdSignalingPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdSignalingMsg1 = true;
}

void LinkDelaySyncIntervalSetting::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || !m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled)
    {
        m_state = STATE_NOT_ENABLED;
    }
    else
    {
        switch(m_state)
        {
        case STATE_NOT_ENABLED:
            if(m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled)
            {
                m_portGlobal->currentLogPdelayReqInterval = m_portGlobal->initialLogPdelayReqInterval;
                m_portGlobal->currentLogSyncInterval = m_portGlobal->initialLogSyncInterval;
                m_portGlobal->pdelayReqInterval.ns = NS_PER_SEC * pow(2, m_portGlobal->initialLogPdelayReqInterval);
                m_portGlobal->pdelayReqInterval.ns_frac = 0;
                m_portGlobal->syncInterval.ns = NS_PER_SEC * pow(2, m_portGlobal->initialLogSyncInterval);
                m_portGlobal->syncInterval.ns_frac = 0;
                m_portGlobal->computeNeighborRateRatio = true;
                m_portGlobal->computeNeighborPropDelay = true;
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
                    m_portGlobal->currentLogPdelayReqInterval = m_portGlobal->initialLogPdelayReqInterval;
                    m_portGlobal->pdelayReqInterval.ns = NS_PER_SEC * pow(2, m_portGlobal->initialLogPdelayReqInterval);
                    m_portGlobal->pdelayReqInterval.ns_frac = 0;
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the Pdelay
                    * requester should stop sending for all practical purposes. */
                    m_portGlobal->pdelayReqInterval.ns = NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetLinkDelayInterval());
                    m_portGlobal->pdelayReqInterval.ns_frac = 0;
                    break;
                }

                switch (m_rcvdSignalingPtr->GetTimeSyncInterval())
                {
                case (-128): /* don’t change the interval */
                    break;
                case 126: /* set interval to initial value */
                    m_portGlobal->currentLogSyncInterval = m_portGlobal->initialLogSyncInterval;
                    m_portGlobal->syncInterval.ns = NS_PER_SEC * pow(2, m_portGlobal->initialLogSyncInterval);
                    m_portGlobal->syncInterval.ns_frac = 0;
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the sender
                    * should stop sending for all practical purposes. */
                    m_portGlobal->syncInterval.ns = NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetTimeSyncInterval());
                    m_portGlobal->syncInterval.ns_frac = 0;
                    break;
                }

                switch (m_rcvdSignalingPtr->GetAnnounceInterval())
                {
                case (-128): /* don’t change the interval */
                    break;
                case 126: /* set interval to initial value */
                    m_portGlobal->currentLogAnnounceInterval = m_portGlobal->initialLogAnnounceInterval;
                    m_portGlobal->announceInterval.ns = NS_PER_SEC * pow(2, m_portGlobal->initialLogAnnounceInterval);
                    m_portGlobal->announceInterval.ns_frac = 0;
                    break;
                default:
                    /* Use indicated value; note that the value of 127 will result in an interval of
                    * 2^127 s, or approximately 5.4 * 10^30 years, which indicates that the sender
                    * should stop sending for all practical purposes. */
                    m_portGlobal->announceInterval.ns = NS_PER_SEC * pow(2, m_rcvdSignalingPtr->GetAnnounceInterval());
                    m_portGlobal->announceInterval.ns_frac = 0;
                    break;
                }

                m_portGlobal->computeNeighborRateRatio = m_rcvdSignalingPtr->GetComputeNeighborRateRatio();
                m_portGlobal->computeNeighborPropDelay = m_rcvdSignalingPtr->GetComputeNeighborPropDelay();

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
