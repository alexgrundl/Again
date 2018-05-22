#ifndef PTPCONFIG_H
#define PTPCONFIG_H

#include "types.h"

#include "timesyncdaemon.h"

class PtpConfig
{

public:

    PtpConfig(TimeSyncDaemon* timeSyncDaemon);


    bool ParseArgs(int argc, char** argv);

private:


    TimeSyncDaemon* m_timeSyncDaemon;


    void PrintUsage(char *arg0);
};

#endif // PTPCONFIG_H
