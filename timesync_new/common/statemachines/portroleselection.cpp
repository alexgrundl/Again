#include "portroleselection.h"

PortRoleSelection::PortRoleSelection(TimeAwareSystem *timeAwareSystem, std::vector<INetPort*> ports) : StateMachineBase(timeAwareSystem)
{
    m_ports = ports;
}

PortRoleSelection::~PortRoleSelection()
{

}


void PortRoleSelection::UpdtRoleDisabledTree()
{
    PriorityVector lastGmPriority;
    for (std::vector<PortRole>::size_type i = 0; i < m_ports.size(); ++i)
    {
        m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_DISABLED);

        lastGmPriority.identity.priority1 = 255;
        lastGmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
        lastGmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
        lastGmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
        lastGmPriority.identity.priority2 = 255;
        memset(lastGmPriority.identity.clockIdentity, 255, sizeof(lastGmPriority.identity.clockIdentity));
        lastGmPriority.stepsRemoved = UINT16_MAX;
        memset(lastGmPriority.sourcePortIdentity.clockIdentity, 255, sizeof(lastGmPriority.sourcePortIdentity.clockIdentity));
        lastGmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
        lastGmPriority.portNumber = UINT16_MAX;

        m_timeAwareSystem->SetLastGmPriority(lastGmPriority);

        m_timeAwareSystem->ClearPathTrace();
        m_timeAwareSystem->AddPath(m_timeAwareSystem->GetClockIdentity());
    }
}

void PortRoleSelection::ClearReselectTree()
{
    for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
        m_timeAwareSystem->GetSystemPort(i)->SetReselect(false);
}

void PortRoleSelection::SetSelectedTree()
{
    for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
        m_timeAwareSystem->GetSystemPort(i)->SetSelected(true);
}

void PortRoleSelection::UpdtRolesTree()
{
    bool portIsSlave = false;
    /* Computes the gmPathPriorityVector for each port that has a portPriorityVector and for which neither
     * announce receipt timeout nor, if gmPresent is TRUE, sync receipt timeout have occurred, */

    /* Saves gmPriority (see 10.3.8.19) in lastGmPriority (see 10.3.8.20), computes the gmPriorityVector
     * for the time-aware system and saves it in gmPriority, chosen as the best of the set consisting of the
     * systemPriorityVector (for this time-aware system) and the gmPathPriorityVector for each port for
     * which the clockIdentity of the master port is not equal to thisClock (see 10.2.3.22), */

    SystemPort* bestPort = NULL;
    PriorityVector bestGmPriority = m_timeAwareSystem->GetSystemPriority();

    for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
    {
        SystemPort* port = m_timeAwareSystem->GetSystemPort(i);
        PriorityVector gmPathPriortiy = port->GetPortPriority();
        gmPathPriortiy.stepsRemoved++;

        if(memcmp(gmPathPriortiy.sourcePortIdentity.clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH) != 0 &&
                bestGmPriority.Compare(gmPathPriortiy) == SystemIdentity::INFO_INFERIOR)
        {
            bestGmPriority = gmPathPriortiy;
            bestPort = port;
        }
    }
    m_timeAwareSystem->SetLastGmPriority(m_timeAwareSystem->GetGmPriority());
    m_timeAwareSystem->SetGmPriority(bestGmPriority);

    /* Sets the per-time-aware system global variables leap61, leap59, currentUtcOffsetValid, timeTraceable, frequencyTraceable, currentUtcOffset,
     * and timeSource as follows:
     * 1) If the gmPriorityVector was set to the gmPathPriorityVector of one of the ports, then leap61, leap59, currentUtcOffsetValid, timeTraceable,
     * frequencyTraceable, currentUtcOffset, and timeSource are set to annLeap61, annLeap59, annCurrentUtcOffsetValid, annTimeTraceable,
     * annFrequencyTraceable, annCurrentUtcOffset, and annTimeSource, respectively, for that port.
     * 2) If the gmPriorityVector was set to the systemPriorityVector, then leap61, leap59, currentUtcOffsetValid, timeTraceable, frequencyTraceable,
     * currentUtcOffset, and timeSource are set to sysLeap61, sysLeap59, sysCurrentUtcOffsetValid, sysTimeTraceable, sysFrequencyTraceable,
     * sysCurrentUtcOffset, and sysTimeSource, respectively.
     */
    m_timeAwareSystem->SetLeap61(bestPort != NULL ? bestPort->GetAnnLeap61() : m_timeAwareSystem->GetSysLeap61());
    m_timeAwareSystem->SetLeap59(bestPort != NULL ? bestPort->GetAnnLeap59() : m_timeAwareSystem->GetSysLeap59());
    m_timeAwareSystem->SetCurrentUtcOffsetValid(bestPort != NULL ? bestPort->GetAnnCurrentUtcOffsetValid() : m_timeAwareSystem->GetCurrentUtcOffsetValid());
    m_timeAwareSystem->SetTimeTraceable(bestPort != NULL ? bestPort->GetAnnTimeTraceable() : m_timeAwareSystem->GetSysTimeTraceable());
    m_timeAwareSystem->SetFrequencyTraceable(bestPort != NULL ? bestPort->GetAnnFrequencyTraceable() : m_timeAwareSystem->GetSysFrequencyTraceable());
    m_timeAwareSystem->SetCurrentUtcOffset(bestPort != NULL ? bestPort->GetAnnCurrentUtcOffset() : m_timeAwareSystem->GetSysCurrentUtcOffset());
    m_timeAwareSystem->SetTimeSource(bestPort != NULL ? bestPort->GetAnnTimeSource() : m_timeAwareSystem->GetSysTimeSource());

    /* Computes the first three components and the clockIdentity of the fourth component of the
     * masterPriorityVector for each port (i.e., everything except the portNumber of the fourth component) */
    for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
    {
        SystemPort* port = m_timeAwareSystem->GetSystemPort(i);
        PriorityVector portMasterPriority;
        portMasterPriority.identity = m_timeAwareSystem->GetGmPriority().identity;
        portMasterPriority.stepsRemoved = m_timeAwareSystem->GetGmPriority().stepsRemoved;
        memcpy(portMasterPriority.sourcePortIdentity.clockIdentity, m_timeAwareSystem->GetClockIdentity(),
               CLOCK_ID_LENGTH);
        portMasterPriority.sourcePortIdentity.portNumber = port->GetIdentity().portNumber;
        portMasterPriority.portNumber = port->GetIdentity().portNumber;
        port->SetMasterPriority(portMasterPriority);
    }

    /* Computes masterStepsRemoved, which is equal to:
     * 1) messageStepsRemoved (see 10.3.9.7) for the port associated with the gmPriorityVector, incremented by 1, if the gmPriorityVector is not
     * the systemPriorityVector, or
     * 2) 0 if the gmPriorityVector is the systemPriorityVector */
     m_timeAwareSystem->SetMasterStepsRemoved(bestPort != NULL ? bestPort->GetMessageStepsRemoved() + 1 : 0);

     /* assigns the port role for port j and sets selectedRole[j] equal to this port role, as follows, for j = 1, 2, numberPorts:
      * 3) If the port is disabled (infoIs == Disabled), selectedRole[j] is set to DisabledPort.
      * 4) If announce receipt timeout, or sync receipt timeout with gmPresent set to TRUE, have occurred (infoIs = Aged), updtInfo is set
      * to TRUE and selectedRole[j] is set to MasterPort.
      * 5) If the portPriorityVector was derived from another port on the time-aware system or from the time-aware system itself as the root (infoIs == Mine),
      * selectedRole[j] is set to MasterPort. In addition, updtInfo is set to TRUE if the portPriorityVector differs from the masterPriorityVector or
      * portStepsRemoved differs from masterStepsRemoved.
      * 6) If the portPriorityVector was received in an Announce message and announce receipt timeout, or sync receipt timeout with gmPresent TRUE,
      * have not occurred (infoIs == Received), and the gmPriorityVector is now derived from the portPriorityVector, selectedRole[j] is set to SlavePort
      * and updtInfo is set to FALSE.
      * 7) If the portPriorityVector was received in an Announce message and announce receipt timeout, or sync receipt timeout with gmPresent TRUE, have not
      * occurred (infoIs == Received), the gmPriorityVector is not now derived from the portPriorityVector, the masterPriorityVector is not higher than the
      * portPriorityVector, and the sourcePortIdentity component of the portPriorityVector does not reflect another port on the time-aware system,
      * selectedRole[j] is set to PassivePort and updtInfo is set to FALSE.
      * 8) If the portPriorityVector was received in an Announce message and announce receipt timeout, or sync receipt timeout with gmPresent TRUE, have not
      * occurred (infoIs == Received), the gmPriorityVector is not now derived from the portPriorityVector, the masterPriorityVector is not higher than the
      * portPriorityVector, and the sourcePortIdentity component of the portPriorityVector does reflect another port on the time-aware system,
      * selectedRole[j] set to PassivePort and updtInfo is set to FALSE.
      * 9) If the portPriorityVector was received in an Announce message and announce receipt timeout, or sync receipt timeout with gmPresent TRUE, have not
      * occurred (infoIs == Received), the gmPriorityVector is not now derived from the portPriorityVector, and the masterPriorityVector is better than the
      * portPriorityVector, selectedRole[j] set to MasterPort and updtInfo is set to TRUE. */
     for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
     {
         SystemPort* port = m_timeAwareSystem->GetSystemPort(i);
         if(port->GetInfoIs() == SPANNING_TREE_PORT_STATE_DISABLED)
             m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_DISABLED);
         else if(port->GetInfoIs() == SPANNING_TREE_PORT_STATE_AGED)
         {
             m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_MASTER);
             port->SetUpdtInfo(true);
         }
         else if(port->GetInfoIs() == SPANNING_TREE_PORT_STATE_MINE)
         {
             m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_MASTER);
             if(port->GetPortStepsRemoved() != m_timeAwareSystem->GetMasterStepsRemoved() || bestPort == NULL || port != bestPort)
                 port->SetUpdtInfo(true);
         }
         //6), 7), 8) and 9)
         else if(port->GetInfoIs() == SPANNING_TREE_PORT_STATE_RECEIVED)
         {
             if(port == bestPort)
             {
                 m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_SLAVE);
                 port->SetUpdtInfo(false);

                 portIsSlave = true;
             }
             else if(port->GetMasterPriority().Compare(port->GetPortPriority()) != SystemIdentity::INFO_SUPERIOR)
             {

                 m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_PASSIVE);
                 port->SetUpdtInfo(false);
             }
             else
             {
                 m_timeAwareSystem->SetSelectedRole(i + 1, PORT_ROLE_MASTER);
                 port->SetUpdtInfo(true);
             }
         }
     }

     /* Updates gmPresent as follows:
      * 10) gmPresent is set to TRUE if the priority1 field of the rootSystemIdentity of the gmPriorityVector is less than 255.
      * 11) gmPresent is set to FALSE if the priority1 field of the rootSystemIdentity of the gmPriorityVector is equal to 255. */
     m_timeAwareSystem->SetGmPresent(m_timeAwareSystem->GetSystemPriority().identity.priority1 < 255);

     /* Assigns the port role for port 0, and sets selectedRole[0], as follows:
      * 12) if selectedRole[j] is set to SlavePort for any port with portNumber j, j = 1, 2, ..., numberPorts, selectedRole[0] is set to PassivePort.
      * 13) if selectedRole[j] is not set to SlavePort for any port with portNumber j, j = 1, 2, ..., numberPorts, selectereselectdRole[0] is set to SlavePort. */
    m_timeAwareSystem->SetSelectedRole(0, portIsSlave ? PORT_ROLE_PASSIVE : PORT_ROLE_SLAVE);

    /* If the clockIentity member of the systemIdentity (see 10.3.2) member of gmPriority (see 10.3.8.19) is equal to thisClock (see 10.2.3.22), i.e.,
     * if the current time-aware system is the grandmaster, the pathTrace array is set to contain the single element thisClock (see 10.2.3.22). */
    if(bestPort == NULL)
    {
        m_timeAwareSystem->ClearPathTrace();
        m_timeAwareSystem->AddPath(m_timeAwareSystem->GetClockIdentity());
    }

    lognotice("New port roles.");
    for (std::vector<PortRole>::size_type i = 0; i <= m_ports.size(); ++i)
    {
        PortRole portRole = m_timeAwareSystem->GetSelectedRole(i);
        if(portRole != PORT_ROLE_DISABLED)
        {
            if(i > 0)
            {
                lognotice("Domain %u: Port %s: %s", m_timeAwareSystem->GetDomain(), m_ports[i - 1]->GetInterfaceName().c_str(), GetStrPortRole(portRole));
            }
            else
            {
                lognotice("Domain %u: Port %lu: %s", m_timeAwareSystem->GetDomain(), i, GetStrPortRole(portRole));
            }
        }
    }
}

const char* PortRoleSelection::GetStrPortRole(PortRole role)
{
    if(role == PORT_ROLE_DISABLED)
        return "Disabled";
    else if(role == PORT_ROLE_MASTER)
        return "Master";
    else if(role == PORT_ROLE_PASSIVE)
        return "Passive";
    else
        return "Slave";
}

void PortRoleSelection::ProcessState()
{
    bool reselect = false;
    bool resetPortPriority = false;

    if(m_timeAwareSystem->BEGIN)
    {
        UpdtRoleDisabledTree();
        m_state = STATE_INIT_BRIDGE;
    }
    else
    {
        switch(m_state)
        {
        case STATE_INIT_BRIDGE:
            ClearReselectTree();
            UpdtRolesTree();
            SetSelectedTree();
            m_state = STATE_ROLE_SELECTION;
            break;

        case STATE_ROLE_SELECTION:
            for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
            {
                SystemPort* port = m_timeAwareSystem->GetSystemPort(i);
                if(port->GetReselect())
                {
                    reselect = true;
                }
                if(port->GetInfoIs() == SPANNING_TREE_PORT_STATE_AGED)
                {
                    resetPortPriority = true;
                }
            }

            /* We have to reset the port priority if the spanning tree port state turns to "aged", meaning that the announce timeout expired.
             * Otherwise we'd announce with this priority from now on which is the priority of the old master and not of ourselves. */
            if(reselect && resetPortPriority)
            {
                for (std::vector<SystemPort*>::size_type i = 0; i < m_ports.size(); ++i)
                {
                    m_timeAwareSystem->GetSystemPort(i)->ResetPortPriority();
                }
            }

            if(reselect)
            {
                ClearReselectTree();
                UpdtRolesTree();
                SetSelectedTree();
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
