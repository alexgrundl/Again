#include "clockmastersyncreceive.h"

ClockMasterSyncReceive::ClockMasterSyncReceive(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdClockSourceReq = false;
    m_rcvdClockSourceReqPtr = std::unique_ptr<ClockSourceTimeParams>(new ClockSourceTimeParams());
    m_rcvdLocalClockTick = false;

    m_sourceTimeOld.sec = 0;
    m_sourceTimeOld.ns = 0;
    m_sourceTimeOld.ns_frac = 0;

    m_localTimeOld.ns = 0;
    m_localTimeOld.ns_frac = 0;
}

ClockMasterSyncReceive::~ClockMasterSyncReceive()
{

}

void ClockMasterSyncReceive::ComputeGmRateRatio()
{
    if(m_sourceTimeOld.ns > 0 && m_localTimeOld.ns > 0 &&
            m_timeAwareSystem->localTime.ns != m_localTimeOld.ns)
    {
        m_timeAwareSystem->gmRateRatio = (m_rcvdClockSourceReqPtr->sourceTime - m_sourceTimeOld) /
                (m_timeAwareSystem->localTime - m_localTimeOld);
    }

    m_sourceTimeOld = m_rcvdClockSourceReqPtr->sourceTime;
    m_localTimeOld = m_timeAwareSystem->localTime;
}

void ClockMasterSyncReceive::UpdateMasterTime()
{
    if(m_rcvdClockSourceReq)
        m_timeAwareSystem->masterTime = m_rcvdClockSourceReqPtr->sourceTime;
    else if(m_rcvdLocalClockTick)
        m_timeAwareSystem->masterTime += m_timeAwareSystem->localClockTickInterval / m_timeAwareSystem->gmRateRatio;
}

void ClockMasterSyncReceive::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_timeAwareSystem->masterTime.sec = 0;
        m_timeAwareSystem->masterTime.ns = 0;
        m_timeAwareSystem->masterTime.ns_frac = 0;
        m_timeAwareSystem->localTime.ns = 0;
        m_timeAwareSystem->localTime.ns_frac = 0;
        m_timeAwareSystem->clockSourceTimeBaseIndicatorOld = 0;
        m_rcvdClockSourceReq = false;
        m_rcvdLocalClockTick = false;
    }
    else
    {
        if(m_rcvdClockSourceReq || m_rcvdLocalClockTick)
        {
            m_state = STATE_RECEIVE_SOURCE_TIME;
            UpdateMasterTime();
            m_timeAwareSystem->localTime = m_timeAwareSystem->GetCurrentTime();
            if (m_rcvdClockSourceReq)
            {
                ComputeGmRateRatio();
                m_timeAwareSystem->clockSourceTimeBaseIndicatorOld = m_timeAwareSystem->clockSourceTimeBaseIndicator;
                m_timeAwareSystem->clockSourceTimeBaseIndicator = m_rcvdClockSourceReqPtr->timeBaseIndicator;
                m_timeAwareSystem->clockSourceLastGmPhaseChange = m_rcvdClockSourceReqPtr->lastGmPhaseChange;
                m_timeAwareSystem->clockSourceLastGmFreqChange = m_rcvdClockSourceReqPtr->lastGmFreqChange;
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
