#include <iostream>
#include <sys/types.h>
#include <memory>
#include <signal.h>

#include "types.h"

#ifdef __linux__
#include "timesyncdaemonlinux.h"
#else
#include <io.h>
#include "timesyncdaemonwindows.h"
#endif


using namespace std;

void waitForSignals()
{
#ifdef __linux__

    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
    {
        perror("pthread_sigmask()");
        return;
    }

    do {
        sig = 0;

        if (sigwait(&set, &sig) != 0) {
            perror("sigwait()");
            return;
        }

    } while (sig != SIGINT);

#else
    while(true)
        pal::ms_sleep(1000);
#endif
}

int main()
{
    TimeSyncDaemon* timeSyncDaemon;

#ifdef __linux__
    timeSyncDaemon = new TimeSyncDaemonLinux(2);
#else
    timeSyncDaemon = new TimeSyncDaemonWindows(2);
#endif

    if(timeSyncDaemon->LicenseValid())
    {
        timeSyncDaemon->InitalizePorts();
        timeSyncDaemon->InitializeManagers();
        timeSyncDaemon->Start();

        waitForSignals();
    }

    timeSyncDaemon->Stop();
    delete timeSyncDaemon;
//    clockDom0->StopPPS();
//    delete clockDom0;
//    delete clockDom1;

    return 0;
}

