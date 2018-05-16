#ifndef TIMESYNCDAEMONLINUX_H
#define TIMESYNCDAEMONLINUX_H

#ifdef __linux__

#include "licensechecklinux.h"
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include "ptpclocklinux.h"
#include "linuxnetport.h"

#include "timesyncdaemon.h"


class TimeSyncDaemonLinux : public TimeSyncDaemon
{
public:

    TimeSyncDaemonLinux(int nDomains);


    virtual ~TimeSyncDaemonLinux();


    void InitalizePorts();

private:


    void DeleteNetworkPorts();

};

#endif //__linux__

#endif // TIMESYNCDAEMONLINUX_H
