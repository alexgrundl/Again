#include "portroleselection.h"

PortRoleSelection::PortRoleSelection(TimeAwareSystem *timeAwareSystem, std::vector<std::shared_ptr<PortGlobal>> ports) : StateMachineBase(timeAwareSystem)
{
    m_ports = ports;
}

PortRoleSelection::~PortRoleSelection()
{

}


void PortRoleSelection::UpdtRoleDisabledTree()
{
    for (std::vector<PortRole>::size_type i = 0; i < m_timeAwareSystem->selectedRole.size(); ++i)
    {
        m_timeAwareSystem->selectedRole[i] = PORT_ROLE_DISABLED;

        m_timeAwareSystem->lastGmPriority.identity.priority1 = 255;
        m_timeAwareSystem->lastGmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
        m_timeAwareSystem->lastGmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
        m_timeAwareSystem->lastGmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
        m_timeAwareSystem->lastGmPriority.identity.priority2 = 255;
        memset(m_timeAwareSystem->lastGmPriority.identity.clockIdentity, 255, sizeof(m_timeAwareSystem->lastGmPriority.identity.clockIdentity));
        m_timeAwareSystem->lastGmPriority.stepsRemoved = UINT16_MAX;
        memset(m_timeAwareSystem->lastGmPriority.sourcePortIdentity.clockIdentity, 255, sizeof(m_timeAwareSystem->lastGmPriority.sourcePortIdentity.clockIdentity));
        m_timeAwareSystem->lastGmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
        m_timeAwareSystem->lastGmPriority.portNumber = UINT16_MAX;

        m_timeAwareSystem->pathTrace.clear();
        m_timeAwareSystem->pathTrace.push_back(m_timeAwareSystem->thisClock);
    }
}

void PortRoleSelection::ClearReselectTree()
{
    for (std::vector<bool>::size_type i = 0; i < m_timeAwareSystem->reselect.size(); ++i)
        m_timeAwareSystem->reselect[i] = false;
}

void PortRoleSelection::SetSelectedTree()
{
    for (std::vector<bool>::size_type i = 0; i < m_timeAwareSystem->selected.size(); ++i)
        m_timeAwareSystem->selected[i] = false;
}

void PortRoleSelection::UpdtRolesTree()
{
    /* Computes the gmPathPriorityVector for each port that has a portPriorityVector and for which neither
     * announce receipt timeout nor, if gmPresent is TRUE, sync receipt timeout have occurred, */

    /* Saves gmPriority (see 10.3.8.19) in lastGmPriority (see 10.3.8.20), computes the gmPriorityVector
     * for the time-aware system and saves it in gmPriority, chosen as the best of the set consisting of the
     * systemPriorityVector (for this time-aware system) and the gmPathPriorityVector for each port for
     * which the clockIdentity of the master port is not equal to thisClock (see 10.2.3.22), */

    PriorityVector bestGmPriority = m_timeAwareSystem->systemPriority;

    for (std::vector<std::shared_ptr<PortGlobal>>::size_type i = 1; i < m_ports.size(); ++i)
    {
        std::shared_ptr<PortGlobal> port = m_ports[i];
        //port->portPriority
    }
    m_timeAwareSystem->lastGmPriority = m_timeAwareSystem->gmPriority;
}

void PortRoleSelection::ProcessState()
{

}
