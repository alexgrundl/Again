#include "clockslavesync.h"

ClockSlaveSync::ClockSlaveSync(TimeAwareSystem* timeAwareSystem, ClockMasterSyncOffset *clockMasterSyncOffset) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdLocalClockTick = false;
    m_clockMasterSyncOffset = clockMasterSyncOffset;
    m_rcvdPSSyncPtr = new PortSyncSync();
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
    m_rcvdPSSyncPtr->domain = rcvd->domain;

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
                double neighborRateRatio = 1.0;
                UScaledNs delayAsymmetry = {0, 0};

                if(m_rcvdPSSyncPtr->localPortNumber > 0)
                {
                    SystemPort* systemPort = m_timeAwareSystem->FindSystemPort(m_rcvdPSSyncPtr->localPortNumber);
                    if(systemPort != NULL)
                    {
                        neighborPropDelay = systemPort->GetNeighborPropDelay();
                        neighborRateRatio = systemPort->GetNeighborRateRatio();
                        delayAsymmetry = systemPort->GetDelayAsymmetry();
                        if(neighborRateRatio == 0)
                            neighborRateRatio = 1.0;
                    }
                }

                m_timeAwareSystem->SetSyncReceiptTime(m_rcvdPSSyncPtr->preciseOriginTimestamp + m_rcvdPSSyncPtr->followUpCorrectionField +
                        neighborPropDelay * (m_rcvdPSSyncPtr->rateRatio / neighborRateRatio) + delayAsymmetry);
                m_timeAwareSystem->SetSyncReceiptLocalTime(m_rcvdPSSyncPtr->upstreamTxTime + neighborPropDelay / neighborRateRatio + delayAsymmetry / m_rcvdPSSyncPtr->rateRatio);

                m_timeAwareSystem->SetGmTimeBaseIndicator(m_rcvdPSSyncPtr->gmTimeBaseIndicator);
                m_timeAwareSystem->SetLastGmPhaseChange(m_rcvdPSSyncPtr->lastGmPhaseChange);
                m_timeAwareSystem->SetLastGmFreqChange(m_rcvdPSSyncPtr->lastGmFreqChange);
                InvokeApplicationInterfaceFunction (NULL);//ClockTargetPhaseDiscontinuity.result);

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
