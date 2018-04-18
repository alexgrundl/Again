#include "clockmastersyncreceive.h"

ClockMasterSyncReceive::ClockMasterSyncReceive(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdClockSourceReq = false;
    m_rcvdClockSourceReqPtr = new ClockSourceTimeParams();
    m_rcvdLocalClockTick = false;

    m_sourceTimeOld.sec = 0;
    m_sourceTimeOld.ns = 0;
    m_sourceTimeOld.ns_frac = 0;

    m_localTimeOld.ns = 0;
    m_localTimeOld.ns_frac = 0;
}

ClockMasterSyncReceive::~ClockMasterSyncReceive()
{
    delete m_rcvdClockSourceReqPtr;
}

void ClockMasterSyncReceive::ComputeGmRateRatio()
{
    if(m_sourceTimeOld.ns > 0 && m_localTimeOld.ns > 0 &&
            m_timeAwareSystem->GetLocalTime().ns != m_localTimeOld.ns)
    {
        m_timeAwareSystem->SetGmRateRatio((m_rcvdClockSourceReqPtr->sourceTime - m_sourceTimeOld) /
                (m_timeAwareSystem->GetLocalTime() - m_localTimeOld));
    }
    //printf("GMRateRatio: %f\n", m_timeAwareSystem->GetGmRateRatio());

    m_sourceTimeOld = m_rcvdClockSourceReqPtr->sourceTime;
    m_localTimeOld = m_timeAwareSystem->GetLocalTime();
}

void ClockMasterSyncReceive::UpdateMasterTime()
{
    if(m_rcvdClockSourceReq)
    {
        m_timeAwareSystem->SetMasterTime(m_rcvdClockSourceReqPtr->sourceTime);
    }
    else if(m_rcvdLocalClockTick)
        m_timeAwareSystem->IncreaseMasterTime(m_timeAwareSystem->GetLocalClockTickInterval() / m_timeAwareSystem->GetGmRateRatio());
}

int i = 0;
void ClockMasterSyncReceive::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_timeAwareSystem->SetMasterTime({0, 0, 0});
        m_timeAwareSystem->SetLocalTime({0, 0});
        m_timeAwareSystem->SetClockSourceTimeBaseIndicatorOld(0);
        m_rcvdClockSourceReq = false;
        m_rcvdLocalClockTick = false;
    }
    else
    {
        /* Should be invoked elsewhere. Just for testing... */
        ClockSourceTimeParams params;
        m_timeAwareSystem->GetLocalClock()->Invoke(&params);
        SetClockSourceRequest(&params);

        if(m_rcvdClockSourceReq || m_rcvdLocalClockTick)
        {
            m_state = STATE_RECEIVE_SOURCE_TIME;
            UpdateMasterTime();
            //For now as we work with the local clock as master. With GPS, e.g., you'd have to set
            //master time to GPS time and local time to the NIC time corresponding to this GPS time.
            m_timeAwareSystem->SetLocalTime((UScaledNs)m_rcvdClockSourceReqPtr->sourceTime);//m_timeAwareSystem->GetCurrentTime());
            if (m_rcvdClockSourceReq)
            {
                ComputeGmRateRatio();
                m_timeAwareSystem->SetClockSourceTimeBaseIndicatorOld(m_timeAwareSystem->GetClockSourceTimeBaseIndicator());
                m_timeAwareSystem->SetClockSourceTimeBaseIndicator(m_rcvdClockSourceReqPtr->timeBaseIndicator);
                m_timeAwareSystem->SetClockSourceLastGmPhaseChange(m_rcvdClockSourceReqPtr->lastGmPhaseChange);
                m_timeAwareSystem->SetClockSourceLastGmFreqChange(m_rcvdClockSourceReqPtr->lastGmFreqChange);
            }
            m_rcvdClockSourceReq = false;
            m_rcvdLocalClockTick = false;
        }
        else
            m_state = STATE_WAITING;
    }
}

void ClockMasterSyncReceive::SetClockSourceRequest(ClockSourceTimeParams* clockSourceReqPtr)
{
    m_rcvdClockSourceReqPtr->sourceTime = clockSourceReqPtr->sourceTime;
    m_rcvdClockSourceReqPtr->timeBaseIndicator = clockSourceReqPtr->timeBaseIndicator;
    m_rcvdClockSourceReqPtr->lastGmPhaseChange = clockSourceReqPtr->lastGmPhaseChange;
    m_rcvdClockSourceReqPtr->lastGmFreqChange = clockSourceReqPtr->lastGmFreqChange;
    m_rcvdClockSourceReq = true;
}

void ClockMasterSyncReceive::SignalLocalClockUpdate()
{
    m_rcvdLocalClockTick = true;
}
