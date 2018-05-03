#ifndef PORTIPC_H
#define PORTIPC_H

#ifdef __linux__

#include "statemachinebase.h"
#include "linuxsharedmemoryipc.h"
#include "inetport.h"
#include "linuxnetport.h"

class PortIPC : public StateMachineBaseMD
{
public:

    PortIPC(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetPort* networkPort, int domain);


    virtual ~PortIPC();


    void ProcessState();


//    bool Update(int64_t mlPhoffset, int64_t lsPhoffset, FrequencyRatio mlFreqoffset, FrequencyRatio lsFreqoffset,
//                uint64_t localTime, uint32_t syncCount, uint32_t pdelayCount, PortState portState, bool asCapable,
//                SystemTimeBase systemTimeBase, int64_t pdelay);


private:


    PtpClock* m_ptpClock;


    LinuxSharedMemoryIPC* m_ipc;


    uint64_t m_timeSystemPrevious;


    uint64_t m_timeDevicePrevious;


    uint8_t m_domain;


    UScaledNs m_ipcUpdateTime;
};

#endif //__linux__

#endif // PORTIPC_H
