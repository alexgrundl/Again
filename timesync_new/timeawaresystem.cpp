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
    memset(thisClock, 0, sizeof(thisClock));
    clockMasterLogSyncInterval = 0;


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

    systemPriority.identity.priority1 = 255;
    systemPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    systemPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    systemPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    systemPriority.identity.priority2 = 255;
    memset(systemPriority.identity.clockIdentity, 255, sizeof(systemPriority.identity.clockIdentity));
    systemPriority.stepsRemoved = 0;
    memset(systemPriority.sourcePortIdentity.clockIdentity, 255, sizeof(systemPriority.sourcePortIdentity.clockIdentity));
    systemPriority.sourcePortIdentity.portNumber = 0;
    systemPriority.portNumber = 0;

    gmPriority.identity.priority1 = 255;
    gmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    gmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    gmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    gmPriority.identity.priority2 = 255;
    memset(gmPriority.identity.clockIdentity, 255, sizeof(gmPriority.identity.clockIdentity));
    gmPriority.stepsRemoved = UINT16_MAX;
    memset(gmPriority.sourcePortIdentity.clockIdentity, 255, sizeof(gmPriority.sourcePortIdentity.clockIdentity));
    gmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    gmPriority.portNumber = UINT16_MAX;

    lastGmPriority.identity.priority1 = 255;
    lastGmPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    lastGmPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    lastGmPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    lastGmPriority.identity.priority2 = 255;
    memset(lastGmPriority.identity.clockIdentity, 255, sizeof(lastGmPriority.identity.clockIdentity));
    lastGmPriority.stepsRemoved = UINT16_MAX;
    memset(lastGmPriority.sourcePortIdentity.clockIdentity, 255, sizeof(lastGmPriority.sourcePortIdentity.clockIdentity));
    lastGmPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    lastGmPriority.portNumber = UINT16_MAX;
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

