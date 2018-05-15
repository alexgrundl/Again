#ifndef TIMESYNCDAEMON_H
#define TIMESYNCDAEMON_H

#include "types.h"
#include "timeawaresystem.h"
#include "inetport.h"

class TimeSyncDaemon
{
public:

    TimeSyncDaemon();


private:


    std::vector<TimeAwareSystem*> m_timeAwareSystems;

    std::vector<INetPort*> networkPorts;
};

#endif // TIMESYNCDAEMON_H
