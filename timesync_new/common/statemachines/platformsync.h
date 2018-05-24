#ifndef PLATFORMSYNC_H
#define PLATFORMSYNC_H

#include "types.h"
#include "platform.h"
#include "ThreadWrapper.h"
#include "statemachinebase.h"
#include "timecontrol.h"

class PlatformSync : public StateMachineBaseMD
{

public:

    PlatformSync(TimeAwareSystem* timeAwareSystem, SystemPort* port, INetPort* networkPort);

    virtual ~PlatformSync();


    void ProcessState();

private:


    PtpClock* m_ptpClock;


    TimeControl m_timeControl;


    CThreadWrapper<PlatformSync>* m_portThread;


    struct timespec m_tsExtEvent;


    struct timespec m_tsSystem;


    bool m_syntonize;


    bool m_externalTimestampRead;


    uint32_t ReadTimestamp(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);
};

#endif // PLATFORMSYNC_H
