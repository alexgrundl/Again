#include "clockmastersyncoffset.h"

ClockMasterSyncOffset::ClockMasterSyncOffset(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdSyncReceiptTime = false;
}

ClockMasterSyncOffset::~ClockMasterSyncOffset()
{

}

double ClockMasterSyncOffset::ComputeClockSourceFreqOffset()
{
    /** To be implemented... */

    return 1.0;
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
                /* We chose masterTime. Standard says "sourceTime" but I think they meant "masterTime"??? */
                m_timeAwareSystem->SetClockSourcePhaseOffset(m_timeAwareSystem->GetMasterTime() - m_timeAwareSystem->GetSyncReceiptTime());
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

