#include "timeawaresystem.h"

TimeAwareSystem::TimeAwareSystem()
{

    BEGIN = true;
//    clockMasterSyncInterval.ns = 0;
//    clockMasterSyncInterval.ns_frac = 0;
    clockSlaveTime.sec = 0;
    clockSlaveTime.ns = 0;
    clockSlaveTime.ns_frac = 0;
    syncReceiptTime.sec = 0;
    syncReceiptTime.ns = 0;
    syncReceiptTime.ns_frac = 0;
    syncReceiptLocalTime.ns = 0;
    syncReceiptLocalTime.ns_frac = 0;
    clockSourceFreqOffset = 1.0;
    clockSourcePhaseOffset.ns = 0;
    clockSourcePhaseOffset.ns_frac = 0;
    clockSourceTimeBaseIndicator = 0;
    clockSourceTimeBaseIndicatorOld = 0;
    clockSourceLastGmPhaseChange.ns = 0;
    clockSourceLastGmPhaseChange.ns_frac = 0;
    clockSourceLastGmFreqChange = 1.0;
    gmPresent = false;
    gmRateRatio = 1.0;
    gmTimeBaseIndicator = 0;
    lastGmPhaseChange.ns = 0;
    lastGmPhaseChange.ns_frac = 0;
    lastGmFreqChange = 1.0;
    localClockTickInterval.ns = 0;
    localClockTickInterval.ns_frac = 0;
    localTime.ns = 0;
    localTime.ns_frac = 0;
    masterTime.sec = 0;
    masterTime.ns = 0;
    masterTime.ns_frac = 0;
    memset(thisClock, 255, sizeof(thisClock));
    clockMasterLogSyncInterval = -3;


    masterStepsRemoved = 0;
    leap61 = false;
    leap59 = false;
    currentUtcOffsetValid = false;
    timeTraceable = false;
    frequencyTraceable = false;
    currentUtcOffset = 0;
    timeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;
    sysLeap61 = false;
    sysLeap59 = false;
    sysCurrentUTCOffsetValid = false;
    sysTimeTraceable = false;
    sysFrequencyTraceable = false;
    sysCurrentUtcOffset = 0;
    sysTimeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;

    systemPriority.identity.priority1 = 254;
    systemPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    systemPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    systemPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    systemPriority.identity.priority2 = 255;
    memset(systemPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    systemPriority.stepsRemoved = 0;
    memset(systemPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    systemPriority.sourcePortIdentity.portNumber = 0;
    systemPriority.portNumber = 0;

    gmPriority.identity.priority1 = 255;
    gmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    gmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    gmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    gmPriority.identity.priority2 = 255;
    memset(gmPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    gmPriority.stepsRemoved = UINT16_MAX;
    memset(gmPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    gmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    gmPriority.portNumber = UINT16_MAX;

    lastGmPriority.identity.priority1 = 255;
    lastGmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    lastGmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    lastGmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    lastGmPriority.identity.priority2 = 255;
    memset(lastGmPriority.identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    lastGmPriority.stepsRemoved = UINT16_MAX;
    memset(lastGmPriority.sourcePortIdentity.clockIdentity, 255, CLOCK_ID_LENGTH);
    lastGmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    lastGmPriority.portNumber = UINT16_MAX;

    selectedRole.push_back(PORT_ROLE_SLAVE);
}

TimeAwareSystem::~TimeAwareSystem()
{
    ClearPathTrace();
}

ExtendedTimestamp TimeAwareSystem::GetClockSlaveTime()
{
    return clockSlaveTime;
}

void TimeAwareSystem::SetClockSlaveTime(ExtendedTimestamp time)
{
    clockSlaveTime = time;
}

ExtendedTimestamp TimeAwareSystem::GetSyncReceiptTime()
{
    return syncReceiptTime;
}

void TimeAwareSystem::SetSyncReceiptTime(ExtendedTimestamp time)
{
    syncReceiptTime = time;
}

UScaledNs TimeAwareSystem::GetSyncReceiptLocalTime()
{
    return syncReceiptLocalTime;
}

void TimeAwareSystem::SetSyncReceiptLocalTime(UScaledNs time)
{
    syncReceiptLocalTime = time;
}

double TimeAwareSystem::GetClockSourceFreqOffset()
{
    return clockSourceFreqOffset;
}

void TimeAwareSystem::SetClockSourceFreqOffset(double offset)
{
    clockSourceFreqOffset = offset;
}

ScaledNs TimeAwareSystem::GetClockSourcePhaseOffset()
{
    return clockSourcePhaseOffset;
}

void TimeAwareSystem::SetClockSourcePhaseOffset(ScaledNs offset)
{
    clockSourcePhaseOffset = offset;
}

uint16_t TimeAwareSystem::GetClockSourceTimeBaseIndicator()
{
    return clockSourceTimeBaseIndicator;
}

void TimeAwareSystem::SetClockSourceTimeBaseIndicator(uint16_t indicator)
{
    clockSourceTimeBaseIndicator = indicator;
}

uint16_t TimeAwareSystem::GetClockSourceTimeBaseIndicatorOld()
{
    return clockSourceTimeBaseIndicatorOld;
}

void TimeAwareSystem::SetClockSourceTimeBaseIndicatorOld(uint16_t indicator)
{
    clockSourceTimeBaseIndicatorOld = indicator;
}

ScaledNs TimeAwareSystem::GetClockSourceLastGmPhaseChange()
{
    return clockSourceLastGmPhaseChange;
}

void TimeAwareSystem::SetClockSourceLastGmPhaseChange(ScaledNs value)
{
    clockSourceLastGmPhaseChange = value;
}

double TimeAwareSystem::GetClockSourceLastGmFreqChange()
{
    return clockSourceLastGmFreqChange;
}

void TimeAwareSystem::SetClockSourceLastGmFreqChange(double value)
{
    clockSourceLastGmFreqChange = value;
}

UScaledNs TimeAwareSystem::GetCurrentTime()
{
    struct timespec ts;
    UScaledNs uscaled;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    uscaled.ns = (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
    uscaled.ns_frac = 0;

    return uscaled;
}

bool TimeAwareSystem::IsGmPresent()
{
    return gmPresent;
}

void TimeAwareSystem::SetGmPresent(bool present)
{
    gmPresent = present;
}

double TimeAwareSystem::GetGmRateRatio()
{
    return gmRateRatio;
}

void TimeAwareSystem::SetGmRateRatio(double ratio)
{
    gmRateRatio = ratio;
}

uint16_t TimeAwareSystem::GetGmTimeBaseIndicator()
{
    return gmTimeBaseIndicator;
}

void TimeAwareSystem::SetGmTimeBaseIndicator(uint16_t indicator)
{
    gmTimeBaseIndicator = indicator;
}

ScaledNs TimeAwareSystem::GetLastGmPhaseChange()
{
    return lastGmPhaseChange;
}

void TimeAwareSystem::SetLastGmPhaseChange(ScaledNs value)
{
    lastGmPhaseChange = value;
}

double TimeAwareSystem::GetLastGmFreqChange()
{
    return lastGmFreqChange;
}

void TimeAwareSystem::SetLastGmFreqChange(double value)
{
    lastGmFreqChange = value;
}

TimeInterval TimeAwareSystem::GetLocalClockTickInterval()
{
    return localClockTickInterval;
}

void TimeAwareSystem::SetLocalClockTickInterval(TimeInterval interval)
{
    localClockTickInterval = interval;
}

UScaledNs TimeAwareSystem::GetLocalTime()
{
    return localTime;
}

void TimeAwareSystem::SetLocalTime(UScaledNs time)
{
    localTime = time;
}

void TimeAwareSystem::AddSelectedRole(PortRole role)
{
    selectedRole.push_back(role);
}

PortRole TimeAwareSystem::GetSelectedRole(int index)
{
    return (std::vector<PortRole>::size_type)index < selectedRole.size() ? selectedRole[index] : PORT_ROLE_DISABLED;
}

void TimeAwareSystem::SetSelectedRole(int index, PortRole role)
{
    if((std::vector<PortRole>::size_type)index < selectedRole.size())
            selectedRole[index] = role;
}

ExtendedTimestamp TimeAwareSystem::GetMasterTime()
{
    return masterTime;
}

void TimeAwareSystem::SetMasterTime(ExtendedTimestamp time)
{
    masterTime = time;
}

void TimeAwareSystem::IncreaseMasterTime(TimeInterval interval)
{
    masterTime += interval;
}

const uint8_t* TimeAwareSystem::GetClockIdentity()
{
    return thisClock;
}

void TimeAwareSystem::SetClockIdentity(uint8_t* identity)
{
    memcpy(thisClock, identity, CLOCK_ID_LENGTH);
}

void TimeAwareSystem::AddPath(const uint8_t* path)
{
    uint8_t* pathToAdd = new uint8_t[CLOCK_ID_LENGTH];
    memcpy(pathToAdd, path, CLOCK_ID_LENGTH);

    this->pathTrace.push_back(pathToAdd);
}

int8_t TimeAwareSystem::GetClockMasterLogSyncInterval()
{
    return clockMasterLogSyncInterval;
}

void TimeAwareSystem::SetClockMasterLogSyncInterval(int8_t interval)
{
    clockMasterLogSyncInterval = interval;
}

void TimeAwareSystem::ClearPathTrace()
{
    for (std::vector<uint8_t*>::size_type i = 0; i < pathTrace.size(); ++i)
    {
        delete[] pathTrace[i];
    }
    pathTrace.clear();
}

std::vector<uint8_t*> TimeAwareSystem::GetPathTrace()
{
    return pathTrace;
}
