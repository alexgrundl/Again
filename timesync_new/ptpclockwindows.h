#ifndef PTPCLOCKWINDOWS_H
#define PTPCLOCKWINDOWS_H

#include "ptpclock.h"

class PtpClockWindows : public PtpClock
{
public:

    PtpClockWindows();

    virtual ~PtpClockWindows();


    void Invoke(ClockSourceTimeParams* params) {}


    void AdjustPhase(int64_t nanoseconds) {}


    void AdjustFrequency(double ppm) {}


    bool Open(int clockIndex) {}


    void GetTime(struct timespec* ts) {}


    bool GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice) {}


    bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period) {}


    bool StartPPS() {}


    bool StopPPS(int pinIndex, int channel) {}


    bool StopPPS() {}
};

#endif // PTPCLOCKWINDOWS_H
