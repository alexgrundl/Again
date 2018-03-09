#ifndef PTPCLOCK_H
#define PTPCLOCK_H

#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "interfaces.h"

#define CLOCKFD 3
#define FD_TO_CLOCKID(fd) ((~(clockid_t) (fd) << 3) | CLOCKFD)
#define CLOCKID_TO_FD(clk) ((unsigned int) ~((clk) >> 3))

#define ADJ_SETOFFSET   0x100
#define ADJ_NANO        0x2000

class PtpClock : public ClockSourceTime
{
public:

    PtpClock();


    PtpClock(std::string clockPath);


    ~PtpClock();


    void Invoke(ClockSourceTimeParams* params);


    void AdjustPhase(int64_t nanoseconds);


    void AdjustFrequency(double ppm);


    bool Open(std::string clockPath);

private:

    std::string m_clockPath;


    int m_clockFD;
};

#endif // PTPCLOCK_H
