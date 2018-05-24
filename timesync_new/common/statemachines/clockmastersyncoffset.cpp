#include "clockmastersyncoffset.h"

ClockMasterSyncOffset::ClockMasterSyncOffset(TimeAwareSystem* timeAwareSystem) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdSyncReceiptTime = false;
    m_lastSyncReceiptTime = {0, 0, 0};
    m_lastSyncReceiptLocalTime = {0, 0};
    m_timeControl.SetPtpClock(m_timeAwareSystem->GetLocalClock());
}

ClockMasterSyncOffset::~ClockMasterSyncOffset()
{

}

double ClockMasterSyncOffset::ComputeClockSourceFreqOffset()
{
    uint64_t diffLocal;
    double freqOffset = 1.0;
//    /* We use the sync receipt time as master time as we only have the local clock, for now.
//     * So, the result will always be 1.0. If we have another master source (e.g GPS), then
//     * the master time has to be set to the GPS time exactly when the sync arrived. */
    if(m_lastSyncReceiptLocalTime.ns > 0 && m_lastSyncReceiptTime.sec > 0)
    {
//        freqOffset = (m_timeAwareSystem->GetSyncReceiptTime()/*m_timeAwareSystem->GetMasterTime()*/ - m_lastMasterTime) /
//                (m_timeAwareSystem->GetSyncReceiptTime() - m_lastSyncReceiptTime);

        freqOffset = (m_timeAwareSystem->GetSyncReceiptTime() - m_lastSyncReceiptTime) /
                (m_timeAwareSystem->GetSyncReceiptLocalTime() - m_lastSyncReceiptLocalTime);

        diffLocal = m_timeAwareSystem->GetSyncReceiptLocalTime().ns >= m_lastSyncReceiptTime.ns ?
                    m_timeAwareSystem->GetSyncReceiptLocalTime().ns - m_lastSyncReceiptLocalTime.ns :
                    m_lastSyncReceiptLocalTime.ns - m_timeAwareSystem->GetSyncReceiptLocalTime().ns;
        if(diffLocal > NS_PER_SEC)
        {
            lognotice("Time jump detected");
            m_timeAwareSystem->GetLocalClock()->StartPPS();
        }
    }

//    m_lastMasterTime = /*m_timeAwareSystem->GetMasterTime()*/m_timeAwareSystem->GetSyncReceiptTime();
    m_lastSyncReceiptTime = m_timeAwareSystem->GetSyncReceiptTime();
    m_lastSyncReceiptLocalTime = m_timeAwareSystem->GetSyncReceiptLocalTime();

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
//                m_timeAwareSystem->SetClockSourcePhaseOffset
//                        (/*m_timeAwareSystem->GetMasterTime()*/ m_timeAwareSystem->GetSyncReceiptTime() - m_timeAwareSystem->GetSyncReceiptTime());
//                m_timeAwareSystem->SetClockSourceFreqOffset(ComputeClockSourceFreqOffset());

                m_timeAwareSystem->SetClockSourcePhaseOffset(m_timeAwareSystem->GetSyncReceiptTime() - m_timeAwareSystem->GetSyncReceiptLocalTime());
                m_timeAwareSystem->SetClockSourceFreqOffset(ComputeClockSourceFreqOffset());

                if(m_timeAwareSystem->GetDomain() == TimeAwareSystem::GetDomainToSyntonize())
                    m_timeControl.Syntonize(m_timeAwareSystem->GetClockSourcePhaseOffset(), m_timeAwareSystem->GetClockSourceFreqOffset(),
                                            m_timeAwareSystem->GetClockMasterLogSyncInterval());
            }
            else
            {
                if (m_timeAwareSystem->GetClockSourceTimeBaseIndicator() != m_timeAwareSystem->GetClockSourceTimeBaseIndicatorOld())
                {
                    m_timeAwareSystem->SetClockSourcePhaseOffset(m_timeAwareSystem->GetClockSourceLastGmPhaseChange());
                    m_timeAwareSystem->SetClockSourceFreqOffset(m_timeAwareSystem->GetClockSourceLastGmFreqChange());
                }
            }
        }
    }
}

void ClockMasterSyncOffset::SignalSyncReceiptTimeReceive()
{
    m_rcvdSyncReceiptTime = true;
}

