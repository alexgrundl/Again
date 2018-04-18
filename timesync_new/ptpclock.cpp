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

    GetTime(&ts);

    params->sourceTime.sec = ts.tv_sec;
    params->sourceTime.ns = ts.tv_nsec;
    params->sourceTime.ns_frac = 0;
    params->timeBaseIndicator = 0;
    params->lastGmFreqChange = 1.0;
    params->lastGmPhaseChange.ns = 0;
    params->lastGmPhaseChange.ns_frac = 0;
}

void PtpClock::GetTime(struct timespec* ts)
{
    clock_gettime(FD_TO_CLOCKID(m_clockFD), ts);
}

void PtpClock::AdjustPhase(int64_t nanoseconds)
{
//    struct timespec tsOld, tsNew;
    struct timex tx;
    tx.modes = ADJ_SETOFFSET | ADJ_NANO;

    tx.time.tv_sec = nanoseconds > 0 ? nanoseconds / NS_PER_SEC : nanoseconds / NS_PER_SEC - 1;
    tx.time.tv_usec = nanoseconds > 0 ? nanoseconds % NS_PER_SEC : (nanoseconds % NS_PER_SEC) + NS_PER_SEC;

//    GetTime(&tsOld);
    clock_adjtime(FD_TO_CLOCKID(m_clockFD), &tx);
//    GetTime(&tsNew);
//    GetTime(&tsNew);
}

void PtpClock::AdjustFrequency(double ppm)
{
    struct timex tx;
    tx.modes = ADJ_FREQUENCY;
    tx.freq = long(ppm) << 16;
    tx.freq += long(fmodf(ppm, 1.0) * 65536.0);

    clock_adjtime(FD_TO_CLOCKID(m_clockFD), &tx);
}

bool PtpClock::StartPPS(int pinIndex, int channel, struct ptp_clock_time* period)
{
    bool success = false;
    struct ptp_pin_desc desc;
    struct ptp_perout_request peroutRequest;
    struct timespec ts;

    memset(&desc, 0, sizeof(desc));
    desc.func = PTP_PF_PEROUT;
    desc.index = pinIndex;
    desc.chan = channel;

    if(ioctl(m_clockFD, PTP_PIN_SETFUNC, &desc))
        printf("PTP_PIN_SETFUNC SDP%i failed.\n", pinIndex);
    else
    {
        clock_gettime(FD_TO_CLOCKID(m_clockFD), &ts);
        memset(&peroutRequest, 0, sizeof(peroutRequest));
        peroutRequest.index = channel;
        peroutRequest.start.sec = ts.tv_sec + 2;
        peroutRequest.start.nsec = 0;
        peroutRequest.period.sec = period->sec;
        peroutRequest.period.nsec = period->nsec;

        if(ioctl(m_clockFD, PTP_PEROUT_REQUEST, &peroutRequest))
            printf("PTP_PEROUT_REQUEST SDP%i failed.\n", pinIndex);
        else
        {
            printf("Periodic output SDP%i enabled\n", pinIndex);
            success = true;
        }
    }

    return success;
}

bool PtpClock::StopPPS(int pinIndex, int channel)
{
    struct ptp_pin_desc desc;

    memset(&desc, 0, sizeof(desc));
    desc.func = PTP_PF_NONE;
    desc.index = pinIndex;
    desc.chan = channel;

    printf("Periodic output SDP%i disabled\n", pinIndex);
    return ioctl(m_clockFD, PTP_PIN_SETFUNC, &desc) != -1;
}

bool PtpClock::StartPPS()
{
    struct ptp_clock_time periodSDP;

    periodSDP.sec = 1;
    periodSDP.nsec = 0;
    StopPPS(2, 1);

    return StartPPS(2, 1, &periodSDP);
}

bool PtpClock::StopPPS()
{
    return StopPPS(2, 1);
}
