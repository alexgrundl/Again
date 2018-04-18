#ifndef PTPCLOCK_H
#define PTPCLOCK_H

#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ptp_clock.h>
#include <sys/ioctl.h>
#include <string.h>

#include "interfaces.h"

#define CLOCKFD 3
#define FD_TO_CLOCKID(fd) ((~(clockid_t) (fd) << 3) | CLOCKFD)
#define CLOCKID_TO_FD(clk) ((unsigned int) ~((clk) >> 3))

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET   0x100
#endif
#define ADJ_NANO        0x2000

class PtpClock : public ClockSourceTime
{
public:

    PtpClock();


    PtpClock(std::string clockPath);


    virtual ~PtpClock();


    void Invoke(ClockSourceTimeParams* params);


    void AdjustPhase(int64_t nanoseconds);


    void AdjustFrequency(double ppm);


    bool Open(std::string clockPath);


    void GetTime(struct timespec* ts);


    bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period);


    bool StartPPS();


    bool StopPPS(int pinIndex, int channel);


    bool StopPPS();

private:

    std::string m_clockPath;


    int m_clockFD;
};

#endif // PTPCLOCK_H
