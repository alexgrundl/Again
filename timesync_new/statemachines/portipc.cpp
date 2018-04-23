#include "portipc.h"

PortIPC::PortIPC(TimeAwareSystem *timeAwareSystem, PortGlobal *port, INetworkInterfacePort *networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    timeSystemPrevious = 0;
    timeDevicePrevious = 0;

    LinuxIPCArg arg;
    arg.setIfnameAndDomain(((NetworkPort*)networkPort)->GetInterfaceName().c_str(), 0);
    m_ipc.init(&arg);
    m_ipc.set_if_phc_index(((NetworkPort*)networkPort)->GetPtpClockIndex());

    m_ptpClock.Open(((NetworkPort*)networkPort)->GetPtpClockIndex());
}

PortIPC::~PortIPC()
{

}

void PortIPC::ProcessState()
{
//    ExtendedTimestamp tsMasterLocalPhaseOffset;
    int64_t masterLocalPhaseOffset, localSystemPhaseOffset;
    FrequencyRatio masterLocalFrequencyOffset, localSystemFrequencyOffset;
    uint64_t deviceTime, systemTime;
    PortState portState;
    SystemTimeBase timeBase;

    struct timespec tsSystem, tsDevice;

    m_ptpClock.GetSystemAndDeviceTime(&tsSystem, &tsDevice);

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

    localSystemPhaseOffset = deviceTime - systemTime;

    if(timeSystemPrevious > 0 && timeDevicePrevious > 0 && systemTime - timeSystemPrevious > 0)
        localSystemFrequencyOffset = (deviceTime - timeDevicePrevious) / (systemTime - timeSystemPrevious);
    else
        localSystemFrequencyOffset = 1.0;

    timeSystemPrevious = systemTime;
    timeDevicePrevious = deviceTime;


    portState = m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_MASTER ? PTP_MASTER : PTP_SLAVE;
    timeBase = GPTP_CLOCK_REALTIME;

    m_ipc.update(masterLocalPhaseOffset, localSystemPhaseOffset, masterLocalFrequencyOffset, localSystemFrequencyOffset, deviceTime, 0,
                 m_portGlobal->pdelayCount, portState, m_portGlobal->asCapable, timeBase, m_portGlobal->neighborPropDelay.ns);

    m_ipc.update_grandmaster(m_timeAwareSystem->gmPriority.sourcePortIdentity.clockIdentity, 0);
    m_ipc.update_network_interface(m_timeAwareSystem->GetClockIdentity(), m_timeAwareSystem->systemPriority.identity.priority1,
                                   m_timeAwareSystem->systemPriority.identity.clockQuality.clockClass,
                                   m_timeAwareSystem->systemPriority.identity.clockQuality.offsetScaledLogVariance,
                                   m_timeAwareSystem->systemPriority.identity.clockQuality.clockAccuracy,
                                   m_timeAwareSystem->systemPriority.identity.priority2, 0,
                                   0, 0, 0, m_portGlobal->identity.portNumber);
}

//bool Update(int64_t mlPhoffset, int64_t lsPhoffset, FrequencyRatio mlFreqoffset, FrequencyRatio lsFreqoffset,
//            uint64_t localTime, uint32_t syncCount, uint32_t pdelayCount, PortState portState, bool asCapable,
//            SystemTimeBase systemTimeBase, int64_t pdelay)
//{
//    m_ipc.update(mlPhoffset, lsPhoffset, mlFreqoffset, lsFreqoffset, 5, 6, 7,  PTP_SLAVE, true, GPTP_CLOCK_REALTIME, 5);
//}
