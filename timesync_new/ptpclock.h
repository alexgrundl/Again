#ifndef PTPCLOCK_H
#define PTPCLOCK_H

#include <time.h>

#include "interfaces.h"

class PtpClock : public ClockSourceTime
{
public:


    virtual ~PtpClock(){}


    virtual void AdjustPhase(int64_t nanoseconds) = 0;


    virtual void AdjustFrequency(double ppm) = 0;


    virtual bool Open(int clockIndex) = 0;


    virtual void GetTime(struct timespec* ts) = 0;


    virtual bool GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice) = 0;


    virtual bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period) = 0;


    virtual bool StartPPS() = 0;


    virtual bool StopPPS(int pinIndex, int channel) = 0;


    virtual bool StopPPS() = 0;
};

#endif // PTPCLOCK_H
