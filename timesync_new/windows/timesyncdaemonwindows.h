#ifndef TIMESYNCDAEMONWINDOWS_H
#define TIMESYNCDAEMONWINDOWS_H

#ifdef __linux__

#include "licensecheckwindows.h"
#include "ptpclockwindows.h"
#include "netportwindows.h"

#include "timesyncdaemon.h"


class TimeSyncDaemonWindows : public TimeSyncDaemon
{
public:

    TimeSyncDaemonWindows(int nDomains);


    virtual ~TimeSyncDaemonWindows();


    void InitalizePorts();

private:


    void DeleteNetworkPorts();

};

#endif //__linux__

#endif // TIMESYNCDAEMONWINDOWS_H
