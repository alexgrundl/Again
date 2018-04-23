#ifndef PORTIPC_H
#define PORTIPC_H

#include "statemachinebase.h"
#include "linuxsharedmemoryipc.h"

class PortIPC : public StateMachineBaseMD
{
public:

    PortIPC(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort);


    virtual ~PortIPC();


    void ProcessState();


//    bool Update(int64_t mlPhoffset, int64_t lsPhoffset, FrequencyRatio mlFreqoffset, FrequencyRatio lsFreqoffset,
//                uint64_t localTime, uint32_t syncCount, uint32_t pdelayCount, PortState portState, bool asCapable,
//                SystemTimeBase systemTimeBase, int64_t pdelay);


private:


    PtpClock m_ptpClock;


    LinuxSharedMemoryIPC m_ipc;


    uint64_t timeSystemPrevious;


    uint64_t timeDevicePrevious;
};

#endif // PORTIPC_H
