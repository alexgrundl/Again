#include "clockslavesync.h"

ClockSlaveSync::ClockSlaveSync(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdLocalClockTick = false;
    m_rcvdPSSyncPtr = NULL;
}

ClockSlaveSync::~ClockSlaveSync()
{

}

void ClockSlaveSync::UpdateSlaveTime()
{

}

void ClockSlaveSync::InvokeApplicationInterfaceFunction (void* functionName)
{

}

void ClockSlaveSync::ProcessStruct(PortSyncSync* rcvd)
{
    m_rcvdPSSyncPtr = rcvd;
    m_rcvdPSSync = true;
}

void ClockSlaveSync::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_rcvdPSSync = false;
        m_state = STATE_INITIALIZING;
    }
    else
    {
        if(m_rcvdPSSync || m_rcvdLocalClockTick)
        {
            if (m_rcvdPSSync)
            {
                /* Why do we have to use neighborPropDelay and neighborRateRatio?? Thought this is an instance "per time-aware system"??
                Now chose port 0 but there's something wrong, here... */
        //                m_timeAwareSystem->syncReceiptTime = m_rcvdPSSyncPtr->preciseOriginTimestamp + m_rcvdPSSyncPtr->followUpCorrectionField +
        //                m_ports[0].neighborPropDelay * (m_rcvdPSSyncPtr->rateRatio / m_ports[0].neighborRateRatio) + m_ports[0].delayAsymmetry;
        //                m_timeAwareSystem->syncReceiptLocalTime = m_rcvdPSSyncPtr->upstreamTxTime + m_ports[0].neighborPropDelay /
        //                        m_ports[0].neighborRateRatio + m_ports[0].delayAsymmetry / m_rcvdPSSyncPtr->rateRatio;
                /* End why do ... */

                m_timeAwareSystem->gmTimeBaseIndicator = m_rcvdPSSyncPtr->gmTimeBaseIndicator;
                m_timeAwareSystem->lastGmPhaseChange = m_rcvdPSSyncPtr->lastGmPhaseChange;
                m_timeAwareSystem->lastGmFreqChange = m_rcvdPSSyncPtr->lastGmFreqChange;
                InvokeApplicationInterfaceFunction (NULL);//ClockTargetPhaseDiscontinuity.result);
            }
            if(m_rcvdLocalClockTick)
                UpdateSlaveTime();
            m_rcvdPSSync = false;
            m_rcvdLocalClockTick = false;

            m_state = STATE_SEND_SYNC_INDICATION;
        }
    }
}
