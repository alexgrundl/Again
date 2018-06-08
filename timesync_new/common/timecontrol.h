#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <stdlib.h>

#include "ptpclock.h"

class TimeControl
{
public:

    static const int k_timeJumpLimit = NS_PER_SEC / 10;


    TimeControl();


    void SetPtpClock(PtpClock* ptpClock);


    void Syntonize(ScaledNs masterLocalOffset, double remoteLocalRate, int8_t logSyncPerSec);


    int64_t Syntonize(PtpClock* localClock, struct timespec &tsExtEvent, struct timespec &tsSystemOfEvent);

private:


    float m_ppm;


    double m_integral;


    double m_proportional;


    PtpClock* m_ptpClock;


    uint64_t m_oldBestSource;


    int64_t m_frequencyIntergral;


    void ControlTimePPS(int64_t offset, uint64_t ppsSlaveTime, uint64_t ppsRootTime);
};

#endif // TIMECONTROL_H
