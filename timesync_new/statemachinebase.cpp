#include "statemachinebase.h"

StateMachineBase::StateMachineBase(TimeAwareSystem* timeAwareSystem)
{
    m_state = STATE_INITIALIZING;
    m_timeAwareSystem = timeAwareSystem;
}

StateMachineBase::~StateMachineBase()
{
}

TimeAwareSystem* StateMachineBase::GetTimeAwareSystem()
{
    return m_timeAwareSystem;
}

void StateMachineBase::SetTimeAwareSystem(TimeAwareSystem* timeAwareSystem)
{
    m_timeAwareSystem = timeAwareSystem;
}




StateMachineBasePort::StateMachineBasePort(TimeAwareSystem* timeAwareSystem, PortGlobal* portGlobal) :
    StateMachineBase(timeAwareSystem)
{
    m_portGlobal = portGlobal;
}

StateMachineBasePort::~StateMachineBasePort()
{
}




StateMachineBaseMD::StateMachineBaseMD(TimeAwareSystem* timeAwareSystem, PortGlobal* portGlobal, MDGlobal* mdGlobal,
                                       INetworkInterfacePort *networkPort) :
    StateMachineBasePort(timeAwareSystem, portGlobal)
{
    m_mdGlobal = mdGlobal;
    m_networkPort = networkPort;
}

StateMachineBaseMD::~StateMachineBaseMD()
{
}
