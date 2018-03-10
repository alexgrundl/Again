#include "ptpclock.h"

PtpClock::PtpClock()
{
    m_clockFD = -1;
}

PtpClock::PtpClock(std::string clockPath)
{
    Open(clockPath);
}

PtpClock::~PtpClock()
{
    if(m_clockFD != -1)
        close(m_clockFD);
}


bool PtpClock::Open(std::string clockPath)
{
    m_clockPath = clockPath;
    m_clockFD = open(clockPath.c_str(), O_RDWR);

    return m_clockFD != -1;
}

void PtpClock::Invoke(ClockSourceTimeParams* params)
{
    struct timespec ts;

    clock_gettime(FD_TO_CLOCKID(m_clockFD), &ts);
//    printf("sec: %lu; nsec: %lu\n", ts.tv_sec, ts.tv_nsec);
//    AdjustPhase(-2 * NS_PER_SEC);
//    clock_gettime(FD_TO_CLOCKID(m_clockFD), &ts);
//    printf("sec: %lu; nsec: %lu\n", ts.tv_sec, ts.tv_nsec);

    params->sourceTime.sec = ts.tv_sec;
    params->sourceTime.ns = ts.tv_nsec;
    params->sourceTime.ns_frac = 0;
    params->timeBaseIndicator = 0;
    params->lastGmFreqChange = 1.0;
    params->lastGmPhaseChange.ns = 0;
    params->lastGmPhaseChange.ns_frac = 0;
}

void PtpClock::AdjustPhase(int64_t nanoseconds)
{
    struct timex tx;
    tx.modes = ADJ_SETOFFSET | ADJ_NANO;

    tx.time.tv_sec = nanoseconds / NS_PER_SEC;
    tx.time.tv_usec = nanoseconds % NS_PER_SEC;

    clock_adjtime(FD_TO_CLOCKID(m_clockFD), &tx);
}

void PtpClock::AdjustFrequency(double ppm)
{
    struct timex tx;
    tx.modes = ADJ_FREQUENCY;
    tx.freq = long(ppm) << 16;
    tx.freq += long(fmodf(ppm, 1.0) * 65536.0);

    clock_adjtime(FD_TO_CLOCKID(m_clockFD), &tx);
}
