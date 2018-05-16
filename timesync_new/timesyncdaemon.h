#ifndef TIMESYNCDAEMON_H
#define TIMESYNCDAEMON_H

#include "types.h"
#include "timeawaresystem.h"
#include "inetport.h"
#include "licensecheck.h"
#include "statemachinemanager.h"
#include "portmanager.h"

class TimeSyncDaemon
{
public:

    TimeSyncDaemon(int nDomains);


    virtual ~TimeSyncDaemon();


    bool CheckLicense();


    void InitializeManagers();


    void Start();


    virtual void InitalizePorts() = 0;


protected:


    std::vector<TimeAwareSystem*> m_timeAwareSystems;


    std::vector<INetPort*> m_networkPorts;


    std::vector<StateMachineManager*> m_stateManagers;


    std::vector<PortManager*> m_portManagers;


    LicenseCheck* m_licenseCheck;


    bool LicenseValid();


private:

    void DeleteManagers();
};

#endif // TIMESYNCDAEMON_H
