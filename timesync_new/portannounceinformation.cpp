#include "portannounceinformation.h"

PortAnnounceInformation::PortAnnounceInformation(TimeAwareSystem* timeAwareSystem, PortGlobal* port) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_announceReceiptTimeoutTime.ns = 0;
    m_announceReceiptTimeoutTime.ns_frac = 0;
    m_messagePriority.identity.priority1 = 255;
    m_messagePriority.identity.clockQuality.clockClass = CLOCK_CLASS_APPLICATION_LOST;
    m_messagePriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    m_messagePriority.identity.clockQuality.offsetScaledLogVariance = 0;
    m_messagePriority.identity.priority2 = 255;
    memset(m_messagePriority.identity.clockIdentity, 0, sizeof(m_messagePriority.identity.clockIdentity));
    m_messagePriority.stepsRemoved = 0;
    memset(m_messagePriority.sourcePortIdentity.clockIdentity, 0, sizeof(m_messagePriority.sourcePortIdentity.clockIdentity));
    m_messagePriority.sourcePortIdentity.portNumber = 0;
    m_messagePriority.portNumber = 0;
    m_rcvdInfo = MASTER_INFO_OTHER;
}

PortAnnounceInformation::~PortAnnounceInformation()
{

}

PortAnnounceInformation::MasterInfo PortAnnounceInformation::RcvInfo(PtpMessageAnnounce* rcvdAnnouncePtr)
{
    m_messagePriority.identity.priority1 = rcvdAnnouncePtr->GetGrandmasterPriority1();
    m_messagePriority.identity.clockQuality = rcvdAnnouncePtr->GetGrandmasterClockQuality();
    m_messagePriority.identity.priority2 = rcvdAnnouncePtr->GetGrandmasterPriority2();
    rcvdAnnouncePtr->GetGrandmasterIdentity(m_messagePriority.identity.clockIdentity);
    m_messagePriority.stepsRemoved = rcvdAnnouncePtr->GetStepsRemoved();
    m_messagePriority.sourcePortIdentity = rcvdAnnouncePtr->GetSourcePortIdentity();

    /* if the received message conveys the port role MasterPort.....?????????????????????? */
//    if(m_timeAwareSystem->selectedRole[m_portGlobal->thisPort] == PORT_ROLE_MASTER)
//    {
        SystemIdentity::Info info = m_messagePriority.Compare(m_portGlobal->portPriority);
        if(info == SystemIdentity::INFO_SUPERIOR)
            return MASTER_INFO_SUPERIOR;
        else if(info == SystemIdentity::INFO_EQUAL)
            return MASTER_INFO_REPEATED;
        else
            return MASTER_INFO_INFERIOR;

//    }
    return MASTER_INFO_OTHER;
}

void PortAnnounceInformation::RecordOtherAnnounceInfo(PtpMessageAnnounce* rcvdAnnouncePtr)
{
    m_portGlobal->annLeap61 = rcvdAnnouncePtr->GetFlagLeap61();
    m_portGlobal->annLeap59 = rcvdAnnouncePtr->GetFlagLeap59();
    m_portGlobal->annCurrentUtcOffsetValid = rcvdAnnouncePtr->GetCurrentUtcOffsetValid();
    m_portGlobal->annTimeTraceable = rcvdAnnouncePtr->GetTimeTraceable();
    m_portGlobal->annFrequencyTraceable = rcvdAnnouncePtr->GetFrequencyTraceable();
    m_portGlobal->annCurrentUtcOffset = rcvdAnnouncePtr->GetCurrentUtcOffset();
    m_portGlobal->annTimeSource = rcvdAnnouncePtr->GetTimeSource();
}

void PortAnnounceInformation::ProcessState()
{
    if(((!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable) &&
        (m_portGlobal->infoIs != SPANNING_TREE_PORT_STATE_DISABLED) ) ||
            m_timeAwareSystem->BEGIN)
    {
        ExecuteDisabledState();
        m_state = STATE_DISABLED;
    }
    else
    {
        switch(m_state)
        {
        case STATE_DISABLED:
            if(m_portGlobal->rcvdMsg)
                ExecuteDisabledState();
            else if(m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable)
            {
                ExecuteAgedState();
                m_state = STATE_AGED;
            }
            break;
        case STATE_AGED:
            if(m_timeAwareSystem->selected[m_portGlobal->thisPort - 1] && m_portGlobal->updtInfo)
            {
                ExecuteUpdateState();
                m_state = STATE_UPDATE;
            }
            break;
        case STATE_UPDATE:
        case STATE_SUPERIOR_MASTER_PORT:
        case STATE_REPEATED_MASTER_PORT:
        case STATE_INFERIOR_MASTER_OR_OTHER_PORT:
            m_state = STATE_CURRENT;
            break;
        case STATE_CURRENT:
            if(m_portGlobal->rcvdMsg && !m_portGlobal->updtInfo)
            {
                m_rcvdInfo = RcvInfo(m_portGlobal->rcvdAnnouncePtr);
                m_state = STATE_RECEIVE;
            }
            else if(m_timeAwareSystem->selected[m_portGlobal->thisPort - 1] && m_portGlobal->updtInfo)
            {
                ExecuteUpdateState();
                m_state = STATE_UPDATE;
            }
            else if((m_portGlobal->infoIs == SPANNING_TREE_PORT_STATE_RECEIVED) &&
                    (m_timeAwareSystem->GetCurrentTime() >= m_announceReceiptTimeoutTime //||
                     /* Where to get syncReceiptTimeoutTime ????????????????????? */
                     /*(m_timeAwareSystem->GetCurrentTime() >= m_portGlobal->syncReceiptTimeoutTime &&
                    m_timeAwareSystem->gmPresent)*/) && !m_portGlobal->updtInfo && !m_portGlobal->rcvdMsg)
            {
                ExecuteAgedState();
                m_state = STATE_AGED;
            }
            break;
        case STATE_RECEIVE:
            if(m_rcvdInfo == MASTER_INFO_SUPERIOR)
            {
                /* Sending port is new master port */
                m_portGlobal->portPriority = m_messagePriority;
                m_portGlobal->portStepsRemoved = m_portGlobal->rcvdAnnouncePtr->GetStepsRemoved();
                RecordOtherAnnounceInfo(m_portGlobal->rcvdAnnouncePtr);
                m_portGlobal->announceReceiptTimeoutTimeInterval.ns = (uint64_t)m_portGlobal->announceReceiptTimeout * NS_PER_SEC *
                        pow(2, /*16 +*/ m_portGlobal->rcvdAnnouncePtr->GetLogMessageInterval());
                m_portGlobal->announceReceiptTimeoutTimeInterval.ns_frac = 0;
                m_announceReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime() + m_portGlobal->announceReceiptTimeoutTimeInterval;
                m_portGlobal->infoIs = SPANNING_TREE_PORT_STATE_RECEIVED;
                m_timeAwareSystem->reselect[m_portGlobal->thisPort - 1] = true;
                m_timeAwareSystem->selected[m_portGlobal->thisPort - 1] = false;
                m_portGlobal->rcvdMsg = false;
                /*m_rcvdAnnouncePtr = FALSE*/;
                m_state = STATE_SUPERIOR_MASTER_PORT;
            }
            else if(m_rcvdInfo == MASTER_INFO_REPEATED)
            {
                /* Sending port is same master port */
                m_announceReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime() + m_portGlobal->announceReceiptTimeoutTimeInterval;
                m_portGlobal->rcvdMsg = false;
                /*rcvdAnnouncePtr = FALSE;*/
                m_state = STATE_REPEATED_MASTER_PORT;
            }
            else
            {
                m_portGlobal->rcvdMsg = FALSE;
                /*rcvdAnnouncePtr = FALSE;*/
                m_state = STATE_INFERIOR_MASTER_OR_OTHER_PORT;
            }
            break;
        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void PortAnnounceInformation::ExecuteDisabledState()
{
    m_portGlobal->rcvdMsg = false;
    m_announceReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime();
    m_portGlobal->infoIs = SPANNING_TREE_PORT_STATE_DISABLED;
    m_timeAwareSystem->reselect[m_portGlobal->thisPort - 1] = true;
    m_timeAwareSystem->selected[m_portGlobal->thisPort - 1] = false;
}

void PortAnnounceInformation::ExecuteAgedState()
{
    m_portGlobal->infoIs = SPANNING_TREE_PORT_STATE_AGED;
    m_timeAwareSystem->reselect[m_portGlobal->thisPort - 1] = true;
    m_timeAwareSystem->selected[m_portGlobal->thisPort - 1] = false;
}

void PortAnnounceInformation::ExecuteUpdateState()
{
    m_portGlobal->portPriority = m_portGlobal->masterPriority;
    m_portGlobal->portStepsRemoved = m_timeAwareSystem->masterStepsRemoved;
    m_portGlobal->updtInfo = false;
    m_portGlobal->infoIs = SPANNING_TREE_PORT_STATE_MINE;
    m_portGlobal->newInfo = true;
}
