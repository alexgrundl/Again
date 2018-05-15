#ifndef PTPCLOCKLINUX_H
#define PTPCLOCKLINUX_H

#ifdef __linux__

#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ptp_clock.h>
#include <sys/ioctl.h>
#include <string.h>
#include <limits.h>

#include "ptpclock.h"
#include "platform.h"

#define CLOCKFD 3
#define FD_TO_CLOCKID(fd) ((~(clockid_t) (fd) << 3) | CLOCKFD)
#define CLOCKID_TO_FD(clk) ((unsigned int) ~((clk) >> 3))

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET   0x100
#endif
#ifndef ADJ_NANO
#define ADJ_NANO        0x2000
#endif

class PtpClockLinux : public PtpClock
{
public:

    PtpClockLinux();


    virtual ~PtpClockLinux();


    void Invoke(ClockSourceTimeParams* params);


    void AdjustPhase(int64_t nanoseconds);


    void AdjustFrequency(double ppm);


    bool Open(int clockIndex);


    void GetTime(struct timespec* ts);


    void SetTime(struct timespec* ts);


    bool GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice);


    bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period);


    bool StartPPS();


    bool StopPPS(int pinIndex, int channel);


    bool StopPPS();


    bool EnableExternalTimestamp(int pinIndex);


    bool DisableExternalTimestamp(int pinIndex);


    bool ReadExternalTimestamp(struct timespec &tsExtEvent, struct timespec &tsSystemOfEvent);


    PtssType GetPtssType();


    void SetPtssType(PtssType type);


private:

    std::string ptpClockRootPath;


    std::string m_clockPath;


    int m_clockFD;


    PtssType m_ptssType;


    pal::SectionLock_t m_lock;


    bool SetExternalTimestamp(int pinIndex, bool enable);
};

#endif //__linux__

#endif // PTPCLOCKLINUX_H
