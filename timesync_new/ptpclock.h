#ifndef PTPCLOCK_H
#define PTPCLOCK_H

#include <time.h>

#include "types.h"
#include "interfaces.h"

class PtpClock : public ClockSourceTime
{
public:


    enum PtssType
    {
        //Receives PPS signals from the root. Uses this signals to syntonize to the root clock.
        PTSS_TYPE_SLAVE,
        //The root of the platform time synchronization. Sends the PPS signal to the other clocks.
        //Only one clock of this type should exist.
        PTSS_TYPE_ROOT
    };


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


    virtual PtssType GetPtssType() = 0;


    virtual void SetPtssType(PtssType type) = 0;
};

#endif // PTPCLOCK_H
