#ifdef __linux__

#include "portipc.h"

PortIPC::PortIPC(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetPort *networkPort, int domain) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_domain = domain;
    m_timeSystemPrevious = 0;
    m_timeDevicePrevious = 0;

    LinuxIPCArg arg;
    arg.setIfnameAndDomain(((LinuxNetPort*)networkPort)->GetInterfaceName().c_str(), m_domain);
    m_ipc = new LinuxSharedMemoryIPC();
    m_ipc->init(&arg);
    m_ipc->set_if_phc_index(((LinuxNetPort*)networkPort)->GetPtpClockIndex());

    m_ptpClock = ((LinuxNetPort*)networkPort)->GetPtpClock();

    m_ipcUpdateTime = m_timeAwareSystem->ReadCurrentTime() + UScaledNs({NS_PER_SEC / 10, 0});
}

PortIPC::~PortIPC()
{
    delete m_ipc;
}

void PortIPC::ProcessState()
{
    if(m_timeAwareSystem->ReadCurrentTime() >= m_ipcUpdateTime)
    {
        int64_t masterLocalPhaseOffset, localSystemPhaseOffset;
        FrequencyRatio masterLocalFrequencyOffset, localSystemFrequencyOffset;
        uint64_t deviceTime, systemTime;
        PortState portState;
        SystemTimeBase timeBase;
        const uint8_t* masterClockIdentity;

        struct timespec tsSystem, tsDevice;

        m_ptpClock->GetSystemAndDeviceTime(&tsSystem, &tsDevice);

        deviceTime = (uint64_t)tsDevice.tv_sec * NS_PER_SEC + (uint64_t)tsDevice.tv_nsec;
        systemTime = (uint64_t)tsSystem.tv_sec * NS_PER_SEC + tsSystem.tv_nsec;

        if(m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) != PORT_ROLE_MASTER)
        {
            masterLocalPhaseOffset = m_portGlobal->remoteLocalDelta.ns;
            masterLocalFrequencyOffset = m_portGlobal->remoteLocalRate;
        }
        else
        {
            masterLocalPhaseOffset = 0;
            masterLocalFrequencyOffset = 1;
        }

        localSystemPhaseOffset = systemTime - deviceTime;

        if(m_timeSystemPrevious > 0 && m_timeDevicePrevious > 0 && systemTime - m_timeSystemPrevious > 0)
            localSystemFrequencyOffset = (deviceTime - m_timeDevicePrevious) / (systemTime - m_timeSystemPrevious);
        else
            localSystemFrequencyOffset = 1.0;

        m_timeSystemPrevious = systemTime;
        m_timeDevicePrevious = deviceTime;


        portState = m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_MASTER ? PTP_MASTER : PTP_SLAVE;
        timeBase = GPTP_CLOCK_REALTIME;
        masterClockIdentity =  m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_MASTER ?
                    m_timeAwareSystem->GetClockIdentity() : m_timeAwareSystem->GetGmPriority().sourcePortIdentity.clockIdentity;

        m_ipc->update(masterLocalPhaseOffset, localSystemPhaseOffset, masterLocalFrequencyOffset, localSystemFrequencyOffset, deviceTime, 0,
                     m_portGlobal->pdelayCount, portState, m_portGlobal->asCapable, timeBase, m_portGlobal->neighborPropDelay.ns);

        m_ipc->update_grandmaster(const_cast<uint8_t*>(masterClockIdentity), m_domain);
        m_ipc->update_network_interface(m_timeAwareSystem->GetClockIdentity(), m_timeAwareSystem->GetSystemPriority().identity.priority1,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.clockClass,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.offsetScaledLogVariance,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.clockAccuracy,
                                       m_timeAwareSystem->GetSystemPriority().identity.priority2, 0,
                                       0, 0, 0, m_portGlobal->identity.portNumber);

        m_ipcUpdateTime.ns += NS_PER_SEC / 10;
    }
}

//bool Update(int64_t mlPhoffset, int64_t lsPhoffset, FrequencyRatio mlFreqoffset, FrequencyRatio lsFreqoffset,
//            uint64_t localTime, uint32_t syncCount, uint32_t pdelayCount, PortState portState, bool asCapable,
//            SystemTimeBase systemTimeBase, int64_t pdelay)
//{
//    m_ipc.update(mlPhoffset, lsPhoffset, mlFreqoffset, lsFreqoffset, 5, 6, 7,  PTP_SLAVE, true, GPTP_CLOCK_REALTIME, 5);
//}

#endif //__linux__
