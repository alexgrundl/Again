#include "clockmastersyncoffset.h"

ClockMasterSyncOffset::ClockMasterSyncOffset(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdSyncReceiptTime = false;
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
            if(m_timeAwareSystem->selectedRole[0] == PORT_ROLE_PASSIVE)
            {
                /* We chose masterTime. Standard says "sourceTime" but I think they meant "masterTime"??? */
                m_timeAwareSystem->clockSourcePhaseOffset = m_timeAwareSystem->masterTime - m_timeAwareSystem->syncReceiptTime;
                m_timeAwareSystem->clockSourceFreqOffset = ComputeClockSourceFreqOffset();
            }
            else if (m_timeAwareSystem->clockSourceTimeBaseIndicator != m_timeAwareSystem->clockSourceTimeBaseIndicatorOld)
            {
                m_timeAwareSystem->clockSourcePhaseOffset = m_timeAwareSystem->clockSourceLastGmPhaseChange;
                m_timeAwareSystem->clockSourceFreqOffset = m_timeAwareSystem->clockSourceLastGmFreqChange;
            }
        }
    }
}

