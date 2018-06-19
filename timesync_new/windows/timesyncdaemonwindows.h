#ifndef TIMESYNCDAEMONWINDOWS_H
#define TIMESYNCDAEMONWINDOWS_H

#ifndef __linux__

#include "licensecheckwindows.h"
#include "ptpclockwindows.h"
#include "netportwindows.h"

#include "timesyncdaemon.h"


class TimeSyncDaemonWindows : public TimeSyncDaemon
{
public:

    TimeSyncDaemonWindows(PtpConfig* config);


    virtual ~TimeSyncDaemonWindows();


    void InitalizePorts(PtpConfig* config);

private:


    void DeleteNetworkPorts();

};

#endif //__linux__

#endif // TIMESYNCDAEMONWINDOWS_H
