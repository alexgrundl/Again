#include "timeawaresystem.h"

TimeAwareSystem::TimeAwareSystem()
{
    BEGIN = true;
    clockMasterSyncInterval.ns = 0;
    clockMasterSyncInterval.ns_frac = 0;
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

