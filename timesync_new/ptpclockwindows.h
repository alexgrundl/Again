#ifndef PTPCLOCKWINDOWS_H
#define PTPCLOCKWINDOWS_H

#ifndef __linux__

#include "ptpclock.h"

class PtpClockWindows : public PtpClock
{
public:

    PtpClockWindows();

    virtual ~PtpClockWindows();


    void Invoke(ClockSourceTimeParams* params) {}


    void AdjustPhase(int64_t nanoseconds) {}


    void AdjustFrequency(double ppm) {}


    bool Open(int clockIndex) {return true;}


    void GetTime(struct timespec* ts) {}


    bool GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice) {return true;}


    bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period) {return true;}


    bool StartPPS() {return true;}


    bool StopPPS(int pinIndex, int channel) {return true;}


    bool StopPPS() {return true;}
};

#endif //__linux__

#endif // PTPCLOCKWINDOWS_H
