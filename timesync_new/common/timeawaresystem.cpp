#include "timeawaresystem.h"

uint8_t TimeAwareSystem::s_domainToSyntonize = 1;

uint8_t TimeAwareSystem::s_domainToMeasurePDelay = 0;


TimeAwareSystem::TimeAwareSystem()
{

    BEGIN = true;
//    clockMasterSyncInterval.ns = 0;
//    clockMasterSyncInterval.ns_frac = 0;
    m_clockSlaveTime = {0, 0, 0};
    m_syncReceiptTime = {0, 0, 0};
    m_syncReceiptLocalTime = {0, 0};
    m_clockSourceFreqOffset = 1.0;
    m_clockSourcePhaseOffset = {0, 0};
    m_clockSourceTimeBaseIndicator = 0;
    m_clockSourceTimeBaseIndicatorOld = 0;
    m_clockSourceLastGmPhaseChange = {0, 0};
    m_clockSourceLastGmFreqChange = 1.0;
    m_gmPresent = false;
    m_gmRateRatio = 1.0;
    m_gmTimeBaseIndicator = 0;
    m_lastGmPhaseChange = {0, 0};
    m_lastGmFreqChange = 1.0;
    m_localClockTickInterval = {0, 0};
    m_localTime = {0, 0};
    m_masterTime = {0, 0, 0};
    memset(m_thisClock, 255, sizeof(m_thisClock));
    m_clockMasterLogSyncInterval = -3;


    m_masterStepsRemoved = 0;
    m_leap61 = false;
    m_leap59 = false;
    m_currentUtcOffsetValid = false;
    m_timeTraceable = false;
    m_frequencyTraceable = false;
    m_currentUtcOffset = 0;
    m_timeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;
    m_sysLeap61 = false;
    m_sysLeap59 = false;
    m_sysCurrentUTCOffsetValid = false;
    m_sysTimeTraceable = false;
    m_sysFrequencyTraceable = false;
    m_sysCurrentUtcOffset = 0;
    m_sysTimeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;

    m_systemPriority.identity.priority1 = 254;
    m_systemPriority.identity.clockQuality.clockClass = CLOCK_CLASS_DEFAULT;
    m_systemPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    m_systemPriority.identity.clockQuality.offsetScaledLogVariance = 16640;
    m_systemPriority.identity.priority2 = 255;
    memset(m_systemPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_systemPriority.stepsRemoved = 0;
    memset(m_systemPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_systemPriority.sourcePortIdentity.portNumber = 0;
    m_systemPriority.portNumber = 0;

    m_gmPriority.identity.priority1 = 255;
    m_gmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_DEFAULT;
    m_gmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    m_gmPriority.identity.clockQuality.offsetScaledLogVariance = 16640;
    m_gmPriority.identity.priority2 = 255;
    memset(m_gmPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_gmPriority.stepsRemoved = UINT16_MAX;
    memset(m_gmPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_gmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    m_gmPriority.portNumber = UINT16_MAX;

    m_lastGmPriority.identity.priority1 = 255;
    m_lastGmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_DEFAULT;
    m_lastGmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    m_lastGmPriority.identity.clockQuality.offsetScaledLogVariance = 16640;
    m_lastGmPriority.identity.priority2 = 255;
    memset(m_lastGmPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_lastGmPriority.stepsRemoved = UINT16_MAX;
    memset(m_lastGmPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    m_lastGmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    m_lastGmPriority.portNumber = UINT16_MAX;

    m_clockLocal = NULL;
    m_domain = 0;
    m_ctssEnabled = false;
    m_ptssEnabled = false;
    m_timeRelayEnabled = false;

    m_gpsClockState = GPS_CLOCK_STATE_UNKNOWN;
    m_gpsClockFallback.SetAsFallback();

    m_portRole0 = PORT_ROLE_DISABLED;
}

TimeAwareSystem::~TimeAwareSystem()
{
    ClearPathTrace();

    for (std::vector<SystemPort*>::size_type i = 0; i < m_systemPorts.size(); ++i)
    {
        delete m_systemPorts[i];
    }
    m_systemPorts.clear();

//    if(m_domain == 0 && m_clockLocal != NULL)
//        m_clockLocal->StopPPS();
}

ExtendedTimestamp TimeAwareSystem::GetClockSlaveTime()
{
    return m_clockSlaveTime;
}

void TimeAwareSystem::SetClockSlaveTime(ExtendedTimestamp time)
{
    m_clockSlaveTime = time;
}

ExtendedTimestamp TimeAwareSystem::GetSyncReceiptTime()
{
    return m_syncReceiptTime;
}

void TimeAwareSystem::SetSyncReceiptTime(ExtendedTimestamp time)
{
    m_syncReceiptTime = time;
}

UScaledNs TimeAwareSystem::GetSyncReceiptLocalTime()
{
    return m_syncReceiptLocalTime;
}

void TimeAwareSystem::SetSyncReceiptLocalTime(UScaledNs time)
{
    m_syncReceiptLocalTime = time;
}

double TimeAwareSystem::GetClockSourceFreqOffset()
{
    return m_clockSourceFreqOffset;
}

void TimeAwareSystem::SetClockSourceFreqOffset(double offset)
{
    m_clockSourceFreqOffset = offset;
}

ScaledNs TimeAwareSystem::GetClockSourcePhaseOffset()
{
    return m_clockSourcePhaseOffset;
}

void TimeAwareSystem::SetClockSourcePhaseOffset(ScaledNs offset)
{
    m_clockSourcePhaseOffset = offset;
}

uint16_t TimeAwareSystem::GetClockSourceTimeBaseIndicator()
{
    return m_clockSourceTimeBaseIndicator;
}

void TimeAwareSystem::SetClockSourceTimeBaseIndicator(uint16_t indicator)
{
    m_clockSourceTimeBaseIndicator = indicator;
}

uint16_t TimeAwareSystem::GetClockSourceTimeBaseIndicatorOld()
{
    return m_clockSourceTimeBaseIndicatorOld;
}

void TimeAwareSystem::SetClockSourceTimeBaseIndicatorOld(uint16_t indicator)
{
    m_clockSourceTimeBaseIndicatorOld = indicator;
}

ScaledNs TimeAwareSystem::GetClockSourceLastGmPhaseChange()
{
    return m_clockSourceLastGmPhaseChange;
}

void TimeAwareSystem::SetClockSourceLastGmPhaseChange(ScaledNs value)
{
    m_clockSourceLastGmPhaseChange = value;
}

double TimeAwareSystem::GetClockSourceLastGmFreqChange()
{
    return m_clockSourceLastGmFreqChange;
}

void TimeAwareSystem::SetClockSourceLastGmFreqChange(double value)
{
    m_clockSourceLastGmFreqChange = value;
}

UScaledNs TimeAwareSystem::ReadCurrentTime()
{
    struct timespec ts;
    UScaledNs uscaled;

#ifdef __linux__
    PtpClockLinux* clockLinux = (PtpClockLinux*)m_clockLocal;
    clock_gettime(clockLinux->GetSystemClock() == PtpClockLinux::SYSTEM_CLOCK_MONOTONIC_RAW ? CLOCK_MONOTONIC_RAW : CLOCK_REALTIME, &ts);
//    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
    //Windows get time function
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
#endif

    uscaled.ns = (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
    uscaled.ns_frac = 0;

    return uscaled;
}

bool TimeAwareSystem::IsGmPresent()
{
    return m_gmPresent;
}

void TimeAwareSystem::SetGmPresent(bool present)
{
    m_gmPresent = present;
}

double TimeAwareSystem::GetGmRateRatio()
{
    return m_gmRateRatio;
}

void TimeAwareSystem::SetGmRateRatio(double ratio)
{
    m_gmRateRatio = ratio;
}

uint16_t TimeAwareSystem::GetGmTimeBaseIndicator()
{
    return m_gmTimeBaseIndicator;
}

void TimeAwareSystem::SetGmTimeBaseIndicator(uint16_t indicator)
{
    m_gmTimeBaseIndicator = indicator;
}

ScaledNs TimeAwareSystem::GetLastGmPhaseChange()
{
    return m_lastGmPhaseChange;
}

void TimeAwareSystem::SetLastGmPhaseChange(ScaledNs value)
{
    m_lastGmPhaseChange = value;
}

double TimeAwareSystem::GetLastGmFreqChange()
{
    return m_lastGmFreqChange;
}

void TimeAwareSystem::SetLastGmFreqChange(double value)
{
    m_lastGmFreqChange = value;
}

TimeInterval TimeAwareSystem::GetLocalClockTickInterval()
{
    return m_localClockTickInterval;
}

void TimeAwareSystem::SetLocalClockTickInterval(TimeInterval interval)
{
    m_localClockTickInterval = interval;
}

UScaledNs TimeAwareSystem::GetLocalTime()
{
    return m_localTime;
}

void TimeAwareSystem::SetLocalTime(UScaledNs time)
{
    m_localTime = time;
}

PortRole TimeAwareSystem::GetPortRole0()
{
    return m_portRole0;
}

void TimeAwareSystem::SetPortRole0(PortRole portRole)
{
    m_portRole0 = portRole;
}

ExtendedTimestamp TimeAwareSystem::GetMasterTime()
{
    return m_masterTime;
}

void TimeAwareSystem::SetMasterTime(ExtendedTimestamp time)
{
    m_masterTime = time;
}

void TimeAwareSystem::IncreaseMasterTime(TimeInterval interval)
{
    m_masterTime += interval;
}

const uint8_t* TimeAwareSystem::GetClockIdentity()
{
    return m_thisClock;
}

void TimeAwareSystem::SetClockIdentity(uint8_t* identity)
{
    memcpy(m_thisClock, identity, CLOCK_ID_LENGTH);
    memcpy(m_systemPriority.identity.clockIdentity, identity, CLOCK_ID_LENGTH);
}

void TimeAwareSystem::AddPath(const uint8_t* path)
{
    uint8_t* pathToAdd = new uint8_t[CLOCK_ID_LENGTH];
    memcpy(pathToAdd, path, CLOCK_ID_LENGTH);

    this->m_pathTrace.push_back(pathToAdd);
}

int8_t TimeAwareSystem::GetClockMasterLogSyncInterval()
{
    return m_clockMasterLogSyncInterval;
}

void TimeAwareSystem::SetClockMasterLogSyncInterval(int8_t interval)
{
    m_clockMasterLogSyncInterval = interval;
}

void TimeAwareSystem::ClearPathTrace()
{
    for (std::vector<uint8_t*>::size_type i = 0; i < m_pathTrace.size(); ++i)
    {
        delete[] m_pathTrace[i];
    }
    m_pathTrace.clear();
}

std::vector<uint8_t*> TimeAwareSystem::GetPathTrace()
{
    return m_pathTrace;
}

void TimeAwareSystem::InitLocalClock(PtpClock *clock, int clockIndex)
{
    struct timespec ts_workingClock = {0, 0};
    struct timespec tsSystem, tsDevice;

    m_clockLocal = clock;
    m_clockLocal->SetPtssType(PtpClock::PTSS_TYPE_ROOT);
    m_clockLocal->Open(clockIndex);
    /* Call this method to set the system clock (CLOCK_REALTIME or CLOCK_MONOTONIC_RAW) in linux */
    m_clockLocal->GetSystemAndDeviceTime(&tsSystem, &tsDevice);

#ifdef __linux__
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts_workingClock);
#else
    //Get time on windows
#endif

    m_clockLocal->SetTime(&ts_workingClock);
#ifndef __arm__
    if(m_domain == 0)
    {
        //m_clockLocal->StopPPS();
        m_clockLocal->StartPPS();
    }
#endif
}

PtpClock* TimeAwareSystem::GetLocalClock()
{
    return m_clockLocal;
}

uint8_t TimeAwareSystem::GetDomain()
{
    return m_domain;
}

void TimeAwareSystem::SetDomain(uint8_t domain)
{
    m_domain = domain;
}

uint8_t TimeAwareSystem::GetDomainToSyntonize()
{
    return s_domainToSyntonize;
}

void TimeAwareSystem::SetDomainToSyntonize(uint8_t domain)
{
    s_domainToSyntonize = domain;
}

uint8_t TimeAwareSystem::GetDomainToMeasurePDelay()
{
    return s_domainToMeasurePDelay;
}

void TimeAwareSystem::SetDomainToMeasurePDelay(uint8_t domain)
{
    s_domainToMeasurePDelay = domain;
}

uint8_t TimeAwareSystem::GetMasterStepsRemoved()
{
    return m_masterStepsRemoved;
}

void TimeAwareSystem::SetMasterStepsRemoved(uint8_t stepsRemoved)
{
    m_masterStepsRemoved = stepsRemoved;
}

bool TimeAwareSystem::GetLeap61()
{
    return m_leap61;
}

void TimeAwareSystem::SetLeap61(bool enable)
{
    m_leap61 = enable;
}

bool TimeAwareSystem::GetLeap59()
{
    return m_leap59;
}

void TimeAwareSystem::SetLeap59(bool enable)
{
    m_leap59 = enable;
}

bool TimeAwareSystem::GetCurrentUtcOffsetValid()
{
    return m_currentUtcOffsetValid;
}

void TimeAwareSystem::SetCurrentUtcOffsetValid(bool enable)
{
    m_currentUtcOffsetValid = enable;
}

bool TimeAwareSystem::GetTimeTraceable()
{
    return m_timeTraceable;
}

void TimeAwareSystem::SetTimeTraceable(bool enable)
{
    m_timeTraceable = enable;
}

bool TimeAwareSystem::GetFrequencyTraceable()
{
    return m_frequencyTraceable;
}

void TimeAwareSystem::SetFrequencyTraceable(bool enable)
{
    m_frequencyTraceable = enable;
}

int16_t TimeAwareSystem::GetCurrentUtcOffset()
{
    return m_currentUtcOffset;
}

void TimeAwareSystem::SetCurrentUtcOffset(int16_t offset)
{
    m_currentUtcOffset = offset;
}

ClockTimeSource TimeAwareSystem::GetTimeSource()
{
    return m_timeSource;
}

void TimeAwareSystem::SetTimeSource(ClockTimeSource timeSource)
{
    if(m_timeSource != timeSource)
    {
        lognotice("Domain %u: Master clock time source changed: %s --> %s", m_domain,
                  m_timeSource == CLOCK_TIME_SOURCE_GPS ? "GPS" : "Local Oscillator",
                  timeSource == CLOCK_TIME_SOURCE_GPS ? "GPS" : "Local Oscillator");
    }
    m_timeSource = timeSource;
}

bool TimeAwareSystem::GetSysLeap61()
{
    return m_sysLeap61;
}

void TimeAwareSystem::SetSysLeap61(bool enable)
{
    m_sysLeap61 = enable;
}

bool TimeAwareSystem::GetSysLeap59()
{
    return m_sysLeap59;
}

void TimeAwareSystem::SetSysLeap59(bool enable)
{
    m_sysLeap59 = enable;
}

bool TimeAwareSystem::GetSysCurrentUTCOffsetValid()
{
    return m_sysCurrentUTCOffsetValid;
}

void TimeAwareSystem::SetSysCurrentUTCOffsetValid(bool valid)
{
    m_sysCurrentUTCOffsetValid = valid;
}

bool TimeAwareSystem::GetSysTimeTraceable()
{
    return m_sysTimeTraceable;
}

void TimeAwareSystem::SetSysTimeTraceable(bool traceable)
{
    m_sysTimeTraceable = traceable;
}

bool TimeAwareSystem::GetSysFrequencyTraceable()
{
    return m_sysFrequencyTraceable;
}

void TimeAwareSystem::SetSysFrequencyTraceable(bool traceable)
{
    m_sysFrequencyTraceable = traceable;
}

int16_t TimeAwareSystem::GetSysCurrentUtcOffset()
{
    return m_sysCurrentUtcOffset;
}

void TimeAwareSystem::SetSysCurrentUtcOffset(int16_t offset)
{
    m_sysCurrentUtcOffset = offset;
}

ClockTimeSource TimeAwareSystem::GetSysTimeSource()
{
    return m_sysTimeSource;
}

void TimeAwareSystem::SetSysTimeSource(ClockTimeSource timeSource)
{
    m_sysTimeSource = timeSource;
}

PriorityVector TimeAwareSystem::GetSystemPriority()
{
    return m_systemPriority;
}

void TimeAwareSystem::SetSystemPriority(PriorityVector priority)
{
    m_systemPriority = priority;
}

PriorityVector TimeAwareSystem::GetGmPriority()
{
    return m_gmPriority;
}

void TimeAwareSystem::SetGmPriority(PriorityVector priority)
{
    m_gmPriority = priority;
}

PriorityVector TimeAwareSystem::GetLastGmPriority()
{
    return m_lastGmPriority;
}

void TimeAwareSystem::SetLastGmPriority(PriorityVector priority)
{
    m_lastGmPriority = priority;
}

SystemPort* TimeAwareSystem::AddSystemPort(PortIdentity portIdentity)
{
    SystemPort* port = new SystemPort();

    port->SetIdentity(portIdentity);
    m_systemPorts.push_back(port);

    return port;
}

SystemPort* TimeAwareSystem::FindSystemPort(uint16_t portNumber)
{
    SystemPort* systemPort = NULL;

    for (std::vector<SystemPort*>::size_type i = 0; i < m_systemPorts.size(); ++i)
    {
        if(m_systemPorts[i]->GetIdentity().portNumber == portNumber)
        {
            systemPort = m_systemPorts[i];
            break;
        }
    }

    return systemPort;
}

int TimeAwareSystem::GetNSystemPorts()
{
    return m_systemPorts.size();
}

void TimeAwareSystem::SetSystemPriority1(uint8_t priority1)
{
    m_systemPriority.identity.priority1 = priority1;
}

bool TimeAwareSystem::IsCTSSEnabled()
{
    return m_ctssEnabled;
}

void TimeAwareSystem::SetCTSSEnabled(bool enable)
{
    m_ctssEnabled = enable;
}

bool TimeAwareSystem::IsPTSSEnabled()
{
    return m_ptssEnabled;
}

void TimeAwareSystem::SetPTSSEnabled(bool enable)
{
    m_ptssEnabled = enable;
}

bool TimeAwareSystem::IsTimeRelayEnabled()
{
    return m_timeRelayEnabled;
}

void TimeAwareSystem::SetTimeRelayEnabled(bool enable)
{
    m_timeRelayEnabled = enable;
}

void TimeAwareSystem::UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset)
{
    m_gpsClock.UpdateGpsData(gpsTime, gpsSystemTime, utcOffset);
}

void TimeAwareSystem::UpdateFallbackGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset)
{
    m_gpsClockFallback.UpdateGpsData(gpsTime, gpsSystemTime, utcOffset);
}

GpsClockState TimeAwareSystem::UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime)
{
    GpsClockState clockStateGPS = m_gpsClock.UpdateGPSDataFromPPS(ppsDeviceTime, ppsSystemTime);
    GpsClockState clockStateFallback = m_gpsClockFallback.UpdateGPSDataFromPPS(ppsDeviceTime, ppsSystemTime);
    GpsClockState clockState = clockStateGPS != GPS_CLOCK_STATE_UNKNOWN ? clockStateGPS : clockStateFallback;

    if(m_gpsClockState != clockState)
    {
        lognotice("Domain %u: Local GPS clock state changed: %s --> %s", m_domain,
                  m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? "GPS available" : (m_gpsClockState == GPS_CLOCK_STATE_INTERNAL ? "GPS internal" : "GPS unknown"),
                  clockState == GPS_CLOCK_STATE_AVAILABLE ? "GPS available" : (clockState == GPS_CLOCK_STATE_INTERNAL ? "GPS internal" : "GPS unknown"));
        m_gpsClockState = clockState;
    }

    SetSysTimeSource(m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? CLOCK_TIME_SOURCE_GPS : CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR);
    if(m_portRole0 == PORT_ROLE_SLAVE)
        SetTimeSource(m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? CLOCK_TIME_SOURCE_GPS : CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR);
    if(m_gpsClockState == GPS_CLOCK_STATE_UNKNOWN)
    {
        m_currentUtcOffset = 0;
        m_systemPriority.identity.clockQuality.clockClass = CLOCK_CLASS_DEFAULT;
    }
    else
    {
        m_currentUtcOffset = m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? m_gpsClock.GetUtcOffset() : m_gpsClockFallback.GetUtcOffset();
        m_systemPriority.identity.clockQuality.clockClass = m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? CLOCK_CLASS_PRIMARY : CLOCK_CLASS_PRIMARY_LOST;
    }
    m_currentUtcOffsetValid = m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE;


    return m_gpsClockState;
}

bool TimeAwareSystem::GetGPSTime(uint64_t deviceTime, uint64_t* gpsTime)
{
    return  m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? m_gpsClock.GetGPSTime(deviceTime, gpsTime) : m_gpsClockFallback.GetGPSTime(deviceTime, gpsTime);
}

double TimeAwareSystem::GetGpsToDeviceRate()
{
    return m_gpsClockState == GPS_CLOCK_STATE_AVAILABLE ? m_gpsClock.GetGpsToDeviceRate() : m_gpsClockFallback.GetGpsToDeviceRate();
}

GpsClockState TimeAwareSystem::GetGpsClockState()
{
    return m_gpsClockState;
}
