#include "sitesyncsync.h"

SiteSyncSync::SiteSyncSync(TimeAwareSystem* timeAwareSystem, ClockSlaveSync* clockSlaveSync, std::vector<PortSyncSyncSend*>& portSyncSyncSends) :
    StateMachineBase(timeAwareSystem)
{
    m_rcvdPSSync = false;
    m_rcvdPSSyncPtr = NULL;
    m_txPSSyncPtr = NULL;

    m_clockSlaveSync = clockSlaveSync;
    for (std::vector<PortSyncSyncSend*>::size_type i = 0; i < portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends.push_back(portSyncSyncSends[i]);
    }
}

SiteSyncSync::~SiteSyncSync()
{
    delete m_txPSSyncPtr;
}

PortSyncSync* SiteSyncSync::SetPSSyncSend (PortSyncSync* rcvdPSSyncIndPtr)
{
    PortSyncSync* txPSSyncPtr = new PortSyncSync();
    txPSSyncPtr = rcvdPSSyncIndPtr;

    return txPSSyncPtr;
}

void SiteSyncSync::TxPSSync (PortSyncSync* txPSSyncPtr)
{
    m_clockSlaveSync->SetPortSyncSync(txPSSyncPtr);
    for (std::vector<PortSyncSyncSend*>::size_type i = 0; i < m_portSyncSyncSends.size(); ++i)
    {
        m_portSyncSyncSends[i]->ProcessStruct(txPSSyncPtr);
    }

}

void SiteSyncSync::ProcessStruct(PortSyncSync* rcvd)
{
    m_rcvdPSSyncPtr = rcvd;
    m_rcvdPSSync = true;
}

void SiteSyncSync::ProcessState()
{
    if(m_timeAwareSystem->BEGIN)
    {
        m_state = STATE_INITIALIZING;
        m_rcvdPSSync = false;
    }
    else
    {
        if(m_rcvdPSSync && m_timeAwareSystem->selectedRole[m_rcvdPSSyncPtr->localPortNumber] == PORT_ROLE_SLAVE
                &&  m_timeAwareSystem->gmPresent)
        {
            m_rcvdPSSync = false;
            delete m_txPSSyncPtr;
            m_txPSSyncPtr = SetPSSyncSend(m_rcvdPSSyncPtr);
            TxPSSync(m_txPSSyncPtr);

            m_state = STATE_RECEIVING_SYNC;
        }
    }
}
