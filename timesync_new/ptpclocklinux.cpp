#ifdef __linux__

#include "ptpclocklinux.h"

PtpClockLinux::PtpClockLinux()
{
    ptpClockRootPath = "/dev/ptp";
    m_clockFD = -1;
    m_ptssType = PTSS_TYPE_SLAVE;
}

PtpClockLinux::~PtpClockLinux()
{
    if(m_clockFD != -1)
        close(m_clockFD);
}

bool PtpClockLinux::Open(int clockIndex)
{
    m_clockPath = ptpClockRootPath + std::to_string(clockIndex);
    m_clockFD = open(m_clockPath.c_str(), O_RDWR);

    return m_clockFD != -1;
}

void PtpClockLinux::Invoke(ClockSourceTimeParams* params)
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

void PtpClockLinux::GetTime(struct timespec* ts)
{
    clock_gettime(FD_TO_CLOCKID(m_clockFD), ts);
}

void PtpClockLinux::SetTime(struct timespec* ts)
{
    clock_settime(FD_TO_CLOCKID(m_clockFD), ts);
}

void PtpClockLinux::AdjustPhase(int64_t nanoseconds)
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

void PtpClockLinux::AdjustFrequency(double ppm)
{
    struct timex tx;
    tx.modes = ADJ_FREQUENCY;
    tx.freq = long(ppm) << 16;
    tx.freq += long(fmodf(ppm, 1.0) * 65536.0);

    clock_adjtime(FD_TO_CLOCKID(m_clockFD), &tx);
}

bool PtpClockLinux::GetSystemAndDeviceTime(struct timespec* tsSystem, struct timespec* tsDevice)
{
    struct ptp_clock_time *firstTime;
    struct ptp_clock_time *systemTime = NULL, *deviceTime = NULL;
    int64_t interval = LLONG_MAX;
    struct ptp_sys_offset offset;

    memset( &offset, 0, sizeof(offset));
    offset.n_samples = PTP_MAX_SAMPLES;

    if( ioctl( m_clockFD, PTP_SYS_OFFSET, &offset ) == -1 )
        return false;

    firstTime = &offset.ts[0];
    for(uint32_t i = 0; i < offset.n_samples; ++i )
    {
        int64_t intervalTemp;
        ptp_clock_time* systemTime1 = firstTime+2*i;
        ptp_clock_time* deviceTimeTemp = firstTime+2*i+1;
        ptp_clock_time* systemTime2 = firstTime+2*i+2;

        intervalTemp = abs(systemTime2->sec * NS_PER_SEC + systemTime2->nsec - systemTime1->sec * NS_PER_SEC - systemTime1->nsec);
        if( intervalTemp < interval )
        {
            systemTime = systemTime1;
            deviceTime = deviceTimeTemp;
            interval = intervalTemp;
        }
    }

    if (deviceTime)
    {
        tsDevice->tv_sec = deviceTime->sec;
        tsDevice->tv_nsec = deviceTime->nsec;
    }
    if (systemTime)
    {
        tsSystem->tv_sec = systemTime->sec;
        tsSystem->tv_nsec = systemTime->nsec;
    }

    return true;
}

PtpClock::PtssType PtpClockLinux::GetPtssType()
{
    return m_ptssType;
}

void PtpClockLinux::SetPtssType(PtssType type)
{
    m_ptssType = type;
}

bool PtpClockLinux::StartPPS(int pinIndex, int channel, struct ptp_clock_time* period)
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
    {
        logwarning("PTP_PIN_SETFUNC SDP%i failed.\n", pinIndex);
    }
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
        {
            logwarning("PTP_PEROUT_REQUEST SDP%i failed.", pinIndex);
        }
        else
        {
            lognotice("Periodic output SDP%i enabled.", pinIndex);
            success = true;
        }
    }

    return success;
}

bool PtpClockLinux::StopPPS(int pinIndex, int channel)
{
    struct ptp_pin_desc desc;

    memset(&desc, 0, sizeof(desc));
    desc.func = PTP_PF_NONE;
    desc.index = pinIndex;
    desc.chan = channel;

    lognotice("Periodic output SDP%i disabled.", pinIndex);
    return ioctl(m_clockFD, PTP_PIN_SETFUNC, &desc) != -1;
}

bool PtpClockLinux::StartPPS()
{
    bool success;
    struct ptp_clock_time periodSDP;

    periodSDP.sec = 0;
    periodSDP.nsec = 250000000;
    StopPPS(0, 0);
    success = StartPPS(0, 0, &periodSDP);

    periodSDP.sec = 1;
    periodSDP.nsec = 0;
    StopPPS(2, 1);
    success &= StartPPS(2, 1, &periodSDP);

    return success;
}

bool PtpClockLinux::StopPPS()
{
    bool success = false;

    success = StopPPS(0, 0);
    success &= StopPPS(2, 1);

    return success;
}

bool PtpClockLinux::EnableExternalTimestamp(int pinIndex)
{
    return SetExternalTimestamp(pinIndex, true);
}

bool PtpClockLinux::DisableExternalTimestamp(int pinIndex)
{
    return SetExternalTimestamp(pinIndex, false);
}

bool PtpClockLinux::SetExternalTimestamp(int pinIndex, bool enable)
{
    bool success = false;
    struct ptp_extts_request exttsRequest;

    exttsRequest.index = pinIndex;
    exttsRequest.flags = enable ? PTP_ENABLE_FEATURE : 0;
    if (ioctl(m_clockFD, PTP_EXTTS_REQUEST, &exttsRequest))
    {
        logwarning("PTP_EXTTS_REQUEST failed");
    }
    else
    {
        success = true;
        lognotice("%s: External timestamping of SDP%i %s.", m_clockPath.c_str(), pinIndex, enable ? "enabled" : "disabled");
    }

    return success;
}

//    struct ptp_extts_event event;
//    for (; extts; extts--)
//    {
//        cnt = read(fd, &event, sizeof(event));
//        if (cnt != sizeof(event))
//        {
//            perror("read");
//            break;
//        }
//        printf("event index %u at %lld.%09u\n", event.index,
//               event.t.sec, event.t.nsec);
//        fflush(stdout);
//    }

#endif //__linux__
