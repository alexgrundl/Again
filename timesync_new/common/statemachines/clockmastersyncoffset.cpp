#include "clockmastersyncoffset.h"

ClockMasterSyncOffset::ClockMasterSyncOffset(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdSyncReceiptTime = false;
    m_lastMasterTime = {0, 0, 0};
    m_lastSyncReceiptTime = {0, 0, 0};
}

ClockMasterSyncOffset::~ClockMasterSyncOffset()
{

}

double ClockMasterSyncOffset::ComputeClockSourceFreqOffset()
{
    double freqOffset = 1.0;
    /* We use the sync receipt time as master time as we only have the local clock, for now.
     * So, the result will always be 1.0. If we have another master source (e.g GPS), then
     * the master time has to be set to the GPS time exactly when the sync arrived. */
    if(m_lastMasterTime.sec > 0 && m_lastSyncReceiptTime.sec > 0)
    {
        freqOffset = (m_timeAwareSystem->GetSyncReceiptTime()/*m_timeAwareSystem->GetMasterTime()*/ - m_lastMasterTime) /
                (m_timeAwareSystem->GetSyncReceiptTime() - m_lastSyncReceiptTime);
    }

    m_lastMasterTime = /*m_timeAwareSystem->GetMasterTime()*/m_timeAwareSystem->GetSyncReceiptTime();
    m_lastSyncReceiptTime = m_timeAwareSystem->GetSyncReceiptTime();

    return freqOffset;
}

void ClockMasterSyncOffset::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_rcvdSyncReceiptTime = false;
    }
    else
    {
        if(m_rcvdSyncReceiptTime)
        {
            m_state = STATE_SEND_SYNC_INDICATION;
            m_rcvdSyncReceiptTime = false;
            if(m_timeAwareSystem->GetSelectedRole(0) == PORT_ROLE_PASSIVE)
            {
                /* Again for the local clock we have no offset to the master as the local clock is our master up until now... */
                m_timeAwareSystem->SetClockSourcePhaseOffset
                        (/*m_timeAwareSystem->GetMasterTime()*/ m_timeAwareSystem->GetSyncReceiptTime() - m_timeAwareSystem->GetSyncReceiptTime());
                m_timeAwareSystem->SetClockSourceFreqOffset(ComputeClockSourceFreqOffset());
            }
            else if (m_timeAwareSystem->GetClockSourceTimeBaseIndicator() != m_timeAwareSystem->GetClockSourceTimeBaseIndicatorOld())
            {
                m_timeAwareSystem->SetClockSourcePhaseOffset(m_timeAwareSystem->GetClockSourceLastGmPhaseChange());
                m_timeAwareSystem->SetClockSourceFreqOffset(m_timeAwareSystem->GetClockSourceLastGmFreqChange());
            }
        }
    }
}

void ClockMasterSyncOffset::SignalSyncReceiptTimeReceive()
{
    m_rcvdSyncReceiptTime = true;
}

