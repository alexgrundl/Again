#include "ptpmessagepdelayrespfollowup.h"

PtpMessagePDelayRespFollowUp::PtpMessagePDelayRespFollowUp()
{
    m_messageType = PTP_MESSSAGE_TYPE_PDELAY_RESP_FOLLOW_UP;
    m_flags &= ~(1 << FLAG_TIMESCALE);
    m_control = 5;
}

