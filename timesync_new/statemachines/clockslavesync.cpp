#include "clockslavesync.h"

ClockSlaveSync::ClockSlaveSync(TimeAwareSystem* timeAwareSystem, std::vector<PortGlobal*> ports) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdLocalClockTick = false;
    m_rcvdPSSyncPtr = new PortSyncSync();

    m_ports = ports;
}

ClockSlaveSync::~ClockSlaveSync()
{
    delete m_rcvdPSSyncPtr;
}

void ClockSlaveSync::UpdateSlaveTime()
{
    if(m_timeAwareSystem->gmPresent)
    {

    }
    else
        m_timeAwareSystem->clockSlaveTime = m_timeAwareSystem->localTime;
}

void ClockSlaveSync::InvokeApplicationInterfaceFunction (void* functionName)
{

}

void ClockSlaveSync::SetPortSyncSync(PortSyncSync* rcvd)
{
    m_rcvdPSSyncPtr->followUpCorrectionField = rcvd->followUpCorrectionField;
    m_rcvdPSSyncPtr->gmTimeBaseIndicator = rcvd->gmTimeBaseIndicator;
    m_rcvdPSSyncPtr->lastGmFreqChange = rcvd->lastGmFreqChange;
    m_rcvdPSSyncPtr->lastGmPhaseChange = rcvd->lastGmPhaseChange;
    m_rcvdPSSyncPtr->localPortNumber = rcvd->localPortNumber;
    m_rcvdPSSyncPtr->logMessageInterval = rcvd->logMessageInterval;
    m_rcvdPSSyncPtr->preciseOriginTimestamp = rcvd->preciseOriginTimestamp;
    m_rcvdPSSyncPtr->rateRatio = rcvd->rateRatio;
    m_rcvdPSSyncPtr->sourcePortIdentity = rcvd->sourcePortIdentity;
    m_rcvdPSSyncPtr->syncReceiptTimeoutTime = rcvd->syncReceiptTimeoutTime;
    m_rcvdPSSyncPtr->upstreamTxTime = rcvd->upstreamTxTime;
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
                UScaledNs neighborPropDelay = {0, 0};
                double neighborRateRatio = 0;
                UScaledNs delayAsymmetry = {0, 0};

                if(m_rcvdPSSyncPtr->localPortNumber > 0)
                {
                    neighborPropDelay = m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->neighborPropDelay;
                    neighborRateRatio = m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->neighborRateRatio;
                    delayAsymmetry = m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->delayAsymmetry;
                }

                /* Why do we have to use neighborPropDelay and neighborRateRatio?? Thought this is an instance "per time-aware system"?? */
                m_timeAwareSystem->syncReceiptTime = m_rcvdPSSyncPtr->preciseOriginTimestamp + m_rcvdPSSyncPtr->followUpCorrectionField +
                        neighborPropDelay * (m_rcvdPSSyncPtr->rateRatio / neighborRateRatio) + delayAsymmetry;
                m_timeAwareSystem->syncReceiptLocalTime = m_rcvdPSSyncPtr->upstreamTxTime + neighborPropDelay / neighborRateRatio + delayAsymmetry / m_rcvdPSSyncPtr->rateRatio;
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
