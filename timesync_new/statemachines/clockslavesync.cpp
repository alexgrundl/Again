#include "clockslavesync.h"

ClockSlaveSync::ClockSlaveSync(TimeAwareSystem* timeAwareSystem, std::vector<PortGlobal*> ports, ClockMasterSyncOffset *clockMasterSyncOffset) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdLocalClockTick = false;
    m_clockMasterSyncOffset = clockMasterSyncOffset;
    m_rcvdPSSyncPtr = new PortSyncSync();

    m_ports = ports;
    m_lastSyncReceiptTime = {0, 0, 0};
    m_lastSyncReceiptLocalTime = {0, 0};
    m_timeControl.SetPtpClock(m_timeAwareSystem->GetLocalClock());
}

ClockSlaveSync::~ClockSlaveSync()
{
    delete m_rcvdPSSyncPtr;
}

void ClockSlaveSync::UpdateSlaveTime()
{
    if(m_timeAwareSystem->IsGmPresent())
    {

    }
    else
        m_timeAwareSystem->SetClockSlaveTime(m_timeAwareSystem->GetLocalTime());
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
                m_timeAwareSystem->SetSyncReceiptTime(m_rcvdPSSyncPtr->preciseOriginTimestamp + m_rcvdPSSyncPtr->followUpCorrectionField +
                        neighborPropDelay * (m_rcvdPSSyncPtr->rateRatio / neighborRateRatio) + delayAsymmetry);
                m_timeAwareSystem->SetSyncReceiptLocalTime(m_rcvdPSSyncPtr->upstreamTxTime + neighborPropDelay / neighborRateRatio + delayAsymmetry / m_rcvdPSSyncPtr->rateRatio);
                /* End why do ... */

                m_timeAwareSystem->SetGmTimeBaseIndicator(m_rcvdPSSyncPtr->gmTimeBaseIndicator);
                m_timeAwareSystem->SetLastGmPhaseChange(m_rcvdPSSyncPtr->lastGmPhaseChange);
                m_timeAwareSystem->SetLastGmFreqChange(m_rcvdPSSyncPtr->lastGmFreqChange);
                InvokeApplicationInterfaceFunction (NULL);//ClockTargetPhaseDiscontinuity.result);

                if(m_rcvdPSSyncPtr->localPortNumber > 0)
                {
                    m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->remoteLocalDelta = m_timeAwareSystem->GetSyncReceiptTime() - m_timeAwareSystem->GetSyncReceiptLocalTime();
                    m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->remoteLocalRate = m_lastSyncReceiptLocalTime.ns > 0 && (m_timeAwareSystem->GetSyncReceiptLocalTime() - m_lastSyncReceiptLocalTime).ns > 0 ?
                                (m_timeAwareSystem->GetSyncReceiptTime() - m_lastSyncReceiptTime) / (m_timeAwareSystem->GetSyncReceiptLocalTime() - m_lastSyncReceiptLocalTime) : 1.0;
                    m_timeControl.Syntonize(m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->remoteLocalDelta, m_ports[m_rcvdPSSyncPtr->localPortNumber - 1]->remoteLocalRate);
                    m_lastSyncReceiptTime = m_timeAwareSystem->GetSyncReceiptTime();
                    m_lastSyncReceiptLocalTime = m_timeAwareSystem->GetSyncReceiptLocalTime();
                    //printf("Diff Port %u: %li\n", m_rcvdPSSyncPtr->localPortNumber, remoteLocalDelta.ns);
                }

                m_clockMasterSyncOffset->SignalSyncReceiptTimeReceive();
            }
            if(m_rcvdLocalClockTick)
                UpdateSlaveTime();
            m_rcvdPSSync = false;
            m_rcvdLocalClockTick = false;

            m_state = STATE_SEND_SYNC_INDICATION;
        }
    }
}
