#ifndef TIMESYNCDAEMON_H
#define TIMESYNCDAEMON_H

#include "types.h"
#include "timeawaresystem.h"
#include "inetport.h"
#include "licensecheck.h"
#include "statemachinemanager.h"
#include "portmanager.h"
#include "ptpconfig.h"

class TimeSyncDaemon
{
public:

    TimeSyncDaemon(PtpConfig *config);


    virtual ~TimeSyncDaemon();


    bool LicenseValid();


    void InitializeManagers();


    void Start();


    void Stop();


    TimeAwareSystem* GetTimeAwareSystem(int index);


    virtual void InitalizePorts() = 0;


protected:


    std::vector<TimeAwareSystem*> m_timeAwareSystems;


    std::vector<INetPort*> m_networkPorts;


    std::vector<StateMachineManager*> m_stateManagers;


    std::vector<PortManager*> m_portManagers;


    LicenseCheck* m_licenseCheck;


    PtpConfig* m_config;


    void DeleteManagers();


    void SetFeatures(TimeAwareSystem *timeAwareSystem);
};

#endif // TIMESYNCDAEMON_H
