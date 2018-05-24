#ifdef __linux__

#include "ptpclocklinux.h"

PtpClockLinux::PtpClockLinux()
{
    ptpClockRootPath = "/dev/ptp";
    m_clockFD = -1;
    m_ptssType = PTSS_TYPE_SLAVE;
    m_systemClock = SYSTEM_CLOCK_UNKNOWN;
    m_lock = pal::LockedRegionCreate();
}

PtpClockLinux::~PtpClockLinux()
{
    if(m_clockFD != -1)
        close(m_clockFD);

    pal::LockedRegionDelete(m_lock);
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
    pal::LockedRegionEnter(m_lock);

    struct ptp_clock_time *firstTime;
    struct ptp_clock_time *systemTime = NULL, *deviceTime = NULL;
    int64_t interval = LLONG_MAX;
    struct ptp_sys_offset offset;

    memset( &offset, 0, sizeof(offset));
    offset.n_samples = PTP_MAX_SAMPLES;

    if(!GetSystemAndDeviceTime(&offset))
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

    pal::LockedRegionLeave(m_lock);
    return true;
}

bool PtpClockLinux::GetSystemAndDeviceTime(struct ptp_sys_offset* offset)
{
    if (m_systemClock == SYSTEM_CLOCK_UNKNOWN)
    {
        // try to find out, whether b-plus specific ptp ioctl is available
        // do it once
        if( ioctl( m_clockFD, PTP_SYS_OFFSET_MONO, offset ) == -1 )
        {
            // no such ioctl
            // errno.h: ENOTTY  25 // Inappropriate ioctl for device
            if(errno == ENOTTY)
            {
                // do default method: gettimeofday/PTP_SYS_OFFSET
                m_systemClock = SYSTEM_CLOCK_REALTIME;
            }
        }
        else
        {
            // looks like kernel supports b-plus ioctl mono raw
            m_systemClock = SYSTEM_CLOCK_MONOTONIC_RAW;
            lognotice("Kernel supports PTP_SYS_OFFSET_MONO. Switching to: SYSTEM_CLOCK_MONOTONIC_RAW\n");
        }
    }

    return ioctl( m_clockFD, m_systemClock == SYSTEM_CLOCK_MONOTONIC_RAW ? PTP_SYS_OFFSET_MONO : PTP_SYS_OFFSET, offset ) != -1;
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
    struct ptp_pin_desc desc;

    memset(&desc, 0, sizeof(desc));
    desc.func = PTP_PF_EXTTS;
    desc.index = pinIndex;
    desc.chan = 0;

    if(ioctl(m_clockFD, PTP_PIN_SETFUNC, &desc))
    {
        logwarning("PTP_PIN_SETFUNC SDP%i failed.", pinIndex);
    }
    else
    {
        memset(&exttsRequest, 0, sizeof(exttsRequest));
        exttsRequest.index = 0;
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
    }

    return success;
}

bool PtpClockLinux::ReadExternalTimestamp(struct timespec& tsExtEvent, struct timespec& tsSystemOfEvent)
{
    bool success = false;
    struct timeval timeout;
    fd_set set;

    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    FD_ZERO(&set);
    FD_SET(m_clockFD, &set);
    if(select(m_clockFD + 1, &set, NULL, NULL, &timeout) > 0)
    {
        struct ptp_extts_event event;
        struct timespec ts;
        ssize_t cnt;

        cnt = read(m_clockFD, &event, sizeof(event));
        clock_gettime(m_systemClock == SYSTEM_CLOCK_MONOTONIC_RAW ? CLOCK_MONOTONIC_RAW : CLOCK_REALTIME, &ts);
        if (cnt != sizeof(event))
        {
            logerror("Read of external timestamp failed.");
        }
        else
        {
            tsExtEvent.tv_sec = event.t.sec;
            tsExtEvent.tv_nsec = event.t.nsec;

            tsSystemOfEvent.tv_sec = ts.tv_sec;
            tsSystemOfEvent.tv_nsec = ts.tv_nsec;

            success = true;

            //printf("%s: tsExtEvent.tv_sec: %lu, tsExtEvent.tv_nsec: %lu\n", m_clockPath.c_str(), tsExtEvent.tv_sec, tsExtEvent.tv_nsec);
        }
    }
    return success;
}


PtpClockLinux::SystemClock PtpClockLinux::GetSystemClock()
{
    return m_systemClock;
}

#endif //__linux__
