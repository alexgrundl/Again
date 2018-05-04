#include "statemachinebase.h"

StateMachineBase::StateMachineBase(TimeAwareSystem* timeAwareSystem)
{
    m_state = STATE_INITIALIZING;
    m_timeAwareSystem = timeAwareSystem;
    m_lock = pal::LockedRegionCreate();
}

StateMachineBase::~StateMachineBase()
{
    pal::LockedRegionDelete(m_lock);
}

TimeAwareSystem* StateMachineBase::GetTimeAwareSystem()
{
    return m_timeAwareSystem;
}

void StateMachineBase::SetTimeAwareSystem(TimeAwareSystem* timeAwareSystem)
{
    m_timeAwareSystem = timeAwareSystem;
}




StateMachineBasePort::StateMachineBasePort(TimeAwareSystem* timeAwareSystem, SystemPort* portGlobal) :
    StateMachineBase(timeAwareSystem)
{
    m_systemPort = portGlobal;
}

StateMachineBasePort::~StateMachineBasePort()
{
}




StateMachineBaseMD::StateMachineBaseMD(TimeAwareSystem* timeAwareSystem, SystemPort* portGlobal, INetPort *networkPort) :
    StateMachineBasePort(timeAwareSystem, portGlobal)
{
    m_networkPort = networkPort;
}

StateMachineBaseMD::~StateMachineBaseMD()
{
}
