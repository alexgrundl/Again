#ifndef TIMESYNCDAEMONLINUX_H
#define TIMESYNCDAEMONLINUX_H

#ifdef __linux__

#include "licensechecklinux.h"
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include "ptpclocklinux.h"
#include "netportlinux.h"
#include "seriallinux.h"

#include "timesyncdaemon.h"


class TimeSyncDaemonLinux : public TimeSyncDaemon
{
public:

    TimeSyncDaemonLinux(PtpConfig* config);


    virtual ~TimeSyncDaemonLinux();


    void InitalizePorts(PtpConfig* config);

private:


    SerialLinux m_serial;

    void DeleteNetworkPorts();

};

#endif //__linux__

#endif // TIMESYNCDAEMONLINUX_H
