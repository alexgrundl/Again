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


    virtual void SetTime(struct timespec* ts) = 0;


    virtual bool GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice) = 0;


    virtual bool StartPPS(int pinIndex, int channel, struct ptp_clock_time* period) = 0;


    virtual bool StartPPS() = 0;


    virtual bool StopPPS(int pinIndex, int channel) = 0;


    virtual bool StopPPS() = 0;


    virtual PtssType GetPtssType() = 0;


    virtual void SetPtssType(PtssType type) = 0;


    virtual bool EnableExternalTimestamp() = 0;


    virtual bool DisableExternalTimestamp() = 0;


    virtual void SetExternalTimestampPin(int pin) = 0;


    virtual bool ReadExternalTimestamp(struct timespec &tsExtEvent, struct timespec &tsSystemOfEvent) = 0;


    virtual int64_t GetPtssOffset() = 0;


    virtual void SetPtssOffset(int64_t offset) = 0;
};

#endif // PTPCLOCK_H
