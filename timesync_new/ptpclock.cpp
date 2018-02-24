#include "ptpclock.h"

PtpClock::PtpClock()
{

}

void PtpClock::Invoke(ClockSourceTimeParams* params)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    params->sourceTime.sec = ts.tv_sec;
    params->sourceTime.ns = ts.tv_nsec;
    params->sourceTime.ns_frac = 0;
    params->timeBaseIndicator = 0;
    params->lastGmFreqChange = 1.0;
    params->lastGmPhaseChange.ns = 0;
    params->lastGmPhaseChange.ns_frac = 0;
}
