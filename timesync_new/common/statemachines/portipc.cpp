#ifdef __linux__

#include "portipc.h"

PortIPC::PortIPC(TimeAwareSystem *timeAwareSystem, SystemPort *port, INetPort *networkPort, int domain) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_domain = domain;
    m_timeSystemPrevious = 0;
    m_timeDevicePrevious = 0;

    m_ipc = new SharedMemoryIPCLinux();
    m_ipc->init(networkPort->GetInterfaceName(), m_domain);
    m_ipc->set_if_phc_index(((NetPortLinux*)networkPort)->GetPtpClockIndex());

    m_ptpClock = ((NetPortLinux*)networkPort)->GetPtpClock();

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
        PortRole portRole;
        SystemTimeBase timeBase;
        const uint8_t* masterClockIdentity;
        uint16_t systemPortNumber = m_systemPort->GetIdentity().portNumber;

        struct timespec tsSystem, tsDevice;

        m_ptpClock->GetSystemAndDeviceTime(&tsSystem, &tsDevice);

        deviceTime = (uint64_t)tsDevice.tv_sec * NS_PER_SEC + (uint64_t)tsDevice.tv_nsec;
        systemTime = (uint64_t)tsSystem.tv_sec * NS_PER_SEC + tsSystem.tv_nsec;

        //if(m_timeAwareSystem->GetSelectedRole(systemPortNumber) != PORT_ROLE_MASTER)
        if(m_timeAwareSystem->GetSelectedRole(0) == PORT_ROLE_PASSIVE)
        {
            //Values are inverted because "xtss-monitor" interpretation of "masterLocalPhaseOffset" is "local - master time"
            //and "masterLocalFrequencyOffset" is "local / master frequency"...
            if(m_networkPort->GetPtpClock()->GetPtssType() == PtpClock::PTSS_TYPE_ROOT)
                masterLocalPhaseOffset = -m_timeAwareSystem->GetClockSourcePhaseOffset().ns;
            else
                masterLocalPhaseOffset = m_networkPort->GetPtpClock()->GetPtssOffset();
            masterLocalFrequencyOffset = m_timeAwareSystem->GetClockSourceFreqOffset();// != 0 ? 1.0 / m_timeAwareSystem->GetClockSourceFreqOffset() : 1.0;
        }
        else
        {
            if(m_networkPort->GetPtpClock()->GetPtssType() == PtpClock::PTSS_TYPE_ROOT)
                masterLocalPhaseOffset = 0;
            else
                masterLocalPhaseOffset = m_networkPort->GetPtpClock()->GetPtssOffset();

            masterLocalFrequencyOffset = 1.0;
        }

        localSystemPhaseOffset = systemTime - deviceTime;

        if(m_timeSystemPrevious > 0 && m_timeDevicePrevious > 0 && systemTime - m_timeSystemPrevious > 0)
            localSystemFrequencyOffset = (float)(deviceTime - m_timeDevicePrevious) / (float)(systemTime - m_timeSystemPrevious);
        else
            localSystemFrequencyOffset = 1.0;

        m_timeSystemPrevious = systemTime;
        m_timeDevicePrevious = deviceTime;

        portRole = m_timeAwareSystem->GetSelectedRole(systemPortNumber);
        switch(portRole)
        {
        case PORT_ROLE_MASTER:
            portState = PTP_MASTER;
            break;
        case PORT_ROLE_SLAVE:
            portState = PTP_SLAVE;
            break;
        case PORT_ROLE_DISABLED:
        case PORT_ROLE_PASSIVE:
            portState = PTP_SLAVE;//PTP_DISABLED;
            break;
        }

        timeBase = ((PtpClockLinux*)m_ptpClock)->GetSystemClock() == PtpClockLinux::SYSTEM_CLOCK_MONOTONIC_RAW ? GPTP_CLOCK_MONOTONIC_RAW : GPTP_CLOCK_REALTIME;


        masterClockIdentity =  m_timeAwareSystem->GetGmPriority().identity.clockIdentity;

        m_ipc->update(masterLocalPhaseOffset, localSystemPhaseOffset, masterLocalFrequencyOffset, localSystemFrequencyOffset, deviceTime, 0,
                     m_systemPort->GetPdelayCount(), portState, m_systemPort->GetAsCapable(), timeBase, m_systemPort->GetNeighborPropDelay().ns);

        m_ipc->update_grandmaster(const_cast<uint8_t*>(masterClockIdentity), m_domain);
        m_ipc->update_network_interface(m_timeAwareSystem->GetClockIdentity(), m_timeAwareSystem->GetSystemPriority().identity.priority1,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.clockClass,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.offsetScaledLogVariance,
                                       m_timeAwareSystem->GetSystemPriority().identity.clockQuality.clockAccuracy,
                                       m_timeAwareSystem->GetSystemPriority().identity.priority2, 0,
                                       0, 0, 0, systemPortNumber);

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
