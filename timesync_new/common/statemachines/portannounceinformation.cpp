#include "portannounceinformation.h"

PortAnnounceInformation::PortAnnounceInformation(TimeAwareSystem* timeAwareSystem, SystemPort* port) :
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

    m_messagePriority.portNumber = m_systemPort->GetIdentity().portNumber;

    /* if the received message conveys the port role MasterPort.....?????????????????????? */
//    if(m_timeAwareSystem->selectedRole[m_portGlobal->thisPort] == PORT_ROLE_MASTER)
//    {
        SystemIdentity::Info info = m_messagePriority.Compare(m_systemPort->GetPortPriority());
        if(info == SystemIdentity::INFO_SUPERIOR)
            return MASTER_INFO_SUPERIOR;
        else if(info == SystemIdentity::INFO_EQUAL)
        {
            return CheckForNewClockIdentity(rcvdAnnouncePtr);
        }
        else
        {
            /* As we do not want to become master for a short time if the current master announces
               a new system identity which was worse than the one before, we signal that this master
               info was superior. Nevertheless, the "PortRoleSelection" state machine would correctly
               select a new master if this master is worse than another master, now. */
            if(memcmp(m_messagePriority.identity.clockIdentity, m_systemPort->GetPortPriority().identity.clockIdentity, CLOCK_ID_LENGTH) == 0)
                return MASTER_INFO_SUPERIOR;

            return MASTER_INFO_INFERIOR;
        }

//    }
    return MASTER_INFO_OTHER;
}

void PortAnnounceInformation::RecordOtherAnnounceInfo(PtpMessageAnnounce* rcvdAnnouncePtr)
{
    m_systemPort->SetAnnLeap61(rcvdAnnouncePtr->GetFlagLeap61());
    m_systemPort->SetAnnLeap59(rcvdAnnouncePtr->GetFlagLeap59());
    m_systemPort->SetAnnCurrentUtcOffsetValid(rcvdAnnouncePtr->GetCurrentUtcOffsetValid());
    m_systemPort->SetAnnTimeTraceable(rcvdAnnouncePtr->GetTimeTraceable());
    m_systemPort->SetAnnFrequencyTraceable(rcvdAnnouncePtr->GetFrequencyTraceable());
    m_systemPort->SetAnnCurrentUtcOffset(rcvdAnnouncePtr->GetCurrentUtcOffset());
    m_systemPort->SetAnnTimeSource(rcvdAnnouncePtr->GetTimeSource());
}

void PortAnnounceInformation::ProcessState()
{
    if(((!m_systemPort->IsPortEnabled() || !m_systemPort->IsPttPortEnabled() || !m_systemPort->GetAsCapable()) &&
        (m_systemPort->GetInfoIs() != SPANNING_TREE_PORT_STATE_DISABLED) ) ||
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
            if(m_systemPort->GetRcvdMsg())
                ExecuteDisabledState();
            else if(m_systemPort->IsPortEnabled() && m_systemPort->IsPttPortEnabled() && m_systemPort->GetAsCapable())
            {
                ExecuteAgedState();
                m_state = STATE_AGED;
            }
            break;
        case STATE_AGED:
            if(m_systemPort->IsSelected() && m_systemPort->GetUpdtInfo())
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
            if(m_systemPort->GetRcvdMsg() && !m_systemPort->GetUpdtInfo())
            {
                m_rcvdInfo = RcvInfo(m_systemPort->GetRcvdAnnouncePtr());
                m_state = STATE_RECEIVE;
            }
            else if(m_systemPort->IsSelected() && m_systemPort->GetUpdtInfo())
            {
                ExecuteUpdateState();
                m_state = STATE_UPDATE;
            }
            else if((m_systemPort->GetInfoIs() == SPANNING_TREE_PORT_STATE_RECEIVED) &&
                    (m_timeAwareSystem->ReadCurrentTime() >= m_announceReceiptTimeoutTime //||
                     /* Where to get syncReceiptTimeoutTime ????????????????????? */
                     /*(m_timeAwareSystem->GetCurrentTime() >= m_portGlobal->syncReceiptTimeoutTime &&
                    m_timeAwareSystem->gmPresent)*/) && !m_systemPort->GetUpdtInfo() && !m_systemPort->GetRcvdMsg())
            {
                ExecuteAgedState();
                m_state = STATE_AGED;
            }
            break;
        case STATE_RECEIVE:
            if(m_rcvdInfo == MASTER_INFO_SUPERIOR)
            {
                /* Sending port is new master port */
                m_systemPort->SetPortPriority(m_messagePriority);
                m_systemPort->SetPortStepsRemoved(m_systemPort->GetRcvdAnnouncePtr()->GetStepsRemoved());
                RecordOtherAnnounceInfo(m_systemPort->GetRcvdAnnouncePtr());
                uint64_t intervalNs = (uint64_t)m_systemPort->GetAnnounceReceiptTimeout() * NS_PER_SEC * pow(2, /*16 +*/ m_systemPort->GetRcvdAnnouncePtr()->GetLogMessageInterval());
                m_systemPort->SetAnnounceReceiptTimeoutTimeInterval({intervalNs, 0});
                m_announceReceiptTimeoutTime = m_timeAwareSystem->ReadCurrentTime() + m_systemPort->GetAnnounceReceiptTimeoutTimeInterval();
                m_systemPort->SetInfoIs(SPANNING_TREE_PORT_STATE_RECEIVED);
                m_systemPort->SetReselect(true);
                m_systemPort->SetSelected(false);
                m_systemPort->SetRcvdMsg(false);
                /*m_rcvdAnnouncePtr = FALSE*/;
                m_state = STATE_SUPERIOR_MASTER_PORT;
            }
            else if(m_rcvdInfo == MASTER_INFO_REPEATED)
            {
                /* Sending port is same master port */
                m_announceReceiptTimeoutTime = m_timeAwareSystem->ReadCurrentTime() + m_systemPort->GetAnnounceReceiptTimeoutTimeInterval();
                m_systemPort->SetRcvdMsg(false);
                /*rcvdAnnouncePtr = FALSE;*/
                m_state = STATE_REPEATED_MASTER_PORT;
            }
            else
            {
                m_systemPort->SetRcvdMsg(false);
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
    m_systemPort->SetRcvdMsg(false);
    m_announceReceiptTimeoutTime = m_timeAwareSystem->ReadCurrentTime();
    m_systemPort->SetInfoIs(SPANNING_TREE_PORT_STATE_DISABLED);
    m_systemPort->SetReselect(true);
    m_systemPort->SetSelected(false);
}

void PortAnnounceInformation::ExecuteAgedState()
{
    m_systemPort->SetInfoIs(SPANNING_TREE_PORT_STATE_AGED);
    m_systemPort->SetReselect(true);
    m_systemPort->SetSelected(false);
}

void PortAnnounceInformation::ExecuteUpdateState()
{
    m_systemPort->SetPortPriority(m_systemPort->GetMasterPriority());
    m_systemPort->SetPortStepsRemoved(m_timeAwareSystem->GetMasterStepsRemoved());
    m_systemPort->SetUpdtInfo(false);
    m_systemPort->SetInfoIs(SPANNING_TREE_PORT_STATE_MINE);
    m_systemPort->SetNewInfo(true);
}

PortAnnounceInformation::MasterInfo PortAnnounceInformation::CheckForNewClockIdentity(PtpMessageAnnounce* rcvdAnnouncePtr)
{
    SystemIdentity identityMaster, identityMessage;
    PortAnnounceInformation::MasterInfo masterInfo;

    identityMaster = m_systemPort->GetMasterPriority().identity;
    rcvdAnnouncePtr->GetGrandmasterIdentity(identityMessage.clockIdentity);
    identityMessage.clockQuality = rcvdAnnouncePtr->GetGrandmasterClockQuality();
    identityMessage.priority1 = rcvdAnnouncePtr->GetGrandmasterPriority1();
    identityMessage.priority2 = rcvdAnnouncePtr->GetGrandmasterPriority2();

    /* Force update if the signaled time source changes. */
    if(rcvdAnnouncePtr->GetTimeSource() != m_timeAwareSystem->GetTimeSource())
        return rcvdAnnouncePtr->GetTimeSource() < m_timeAwareSystem->GetTimeSource() ? MASTER_INFO_SUPERIOR : MASTER_INFO_SUPERIOR;
    /* Force update if the signaled identity changes. */
    SystemIdentity::Info info = identityMessage.Compare(identityMaster);
    masterInfo = info == SystemIdentity::INFO_EQUAL ? MASTER_INFO_REPEATED : (info == SystemIdentity::INFO_SUPERIOR ? MASTER_INFO_SUPERIOR : MASTER_INFO_SUPERIOR);

    return masterInfo;
}
