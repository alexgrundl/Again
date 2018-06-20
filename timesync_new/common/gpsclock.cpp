#include "gpsclock.h"

GPSClock::GPSClock()
{
    m_gpsLock = pal::LockedRegionCreate();
    m_gpsData = {0, 0, 0, 0, 0, 0};
    m_gpsData.ppsErrors = k_maxGpsErrors;
    m_gpsDataPrevious = {0, 0, 0, 0, 0, 0};
    m_gpsFallbackData = {0, 0, 0, 0, 0, 0};
    m_gpsFallbackData.ppsErrors = k_maxGpsFallbackErrors;
    m_gpsFallbackDataPrevious = {0, 0, 0, 0, 0, 0};
    m_gpsToDeviceRate = 0.0;
}

GPSClock::~GPSClock()
{
    pal::LockedRegionDelete(m_gpsLock);
}

void GPSClock::UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset, bool fallback)
{
    pal::LockedRegionEnter(m_gpsLock);

    GPSSyncData* gpsData = fallback ? &m_gpsFallbackData : &m_gpsData;
    GPSSyncData* gpsDataPrevious = fallback ? &m_gpsFallbackDataPrevious : &m_gpsDataPrevious;

    gpsDataPrevious->gpsTime = gpsData->gpsTime;
    gpsDataPrevious->gpsSystemTime = gpsData->gpsSystemTime;
    gpsDataPrevious->utcOffset = gpsData->utcOffset;

    gpsData->gpsTime = gpsTime;
    gpsData->gpsSystemTime = gpsSystemTime;
    gpsData->utcOffset = utcOffset;

    pal::LockedRegionLeave(m_gpsLock);
}

GpsClockState GPSClock::UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime)
{
    int nWaits = 0;
    int maxWaits = 5;
    GpsClockState clockState;

    while(m_gpsData.gpsSystemTime == m_gpsDataPrevious.gpsSystemTime)
    {
        pal::ms_sleep(20);
        nWaits++;
        if(nWaits >= maxWaits)
            break;
    }

    pal::LockedRegionEnter(m_gpsLock);
    clockState = UpdateGPSDataFromPPS(ppsDeviceTime, ppsSystemTime, false, k_maxGpsErrors);
    if(clockState == GPS_CLOCK_STATE_UNKNOWN)
        clockState = UpdateGPSDataFromPPS(ppsDeviceTime, ppsSystemTime, true, k_maxGpsFallbackErrors);
    else
        m_gpsFallbackData.ppsErrors = 0;
    pal::LockedRegionLeave(m_gpsLock);

    return clockState;
}

GpsClockState GPSClock::UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool fallback, uint32_t maxErrors)
{
    GpsClockState clockState = GPS_CLOCK_STATE_UNKNOWN;
    GPSSyncData* gpsData = fallback ? &m_gpsFallbackData : &m_gpsData;
    GPSSyncData* gpsDataPrevious = fallback ? &m_gpsFallbackDataPrevious : &m_gpsDataPrevious;

    if(gpsData->gpsSystemTime != gpsDataPrevious->gpsSystemTime)
    {
        SetPPSTimes(ppsDeviceTime, ppsSystemTime, false, fallback);
        clockState = CalculateGpsToDeviceRate(fallback);
        SetPPSTimes(ppsDeviceTime, ppsSystemTime, true, fallback);
    }
    if(clockState == GPS_CLOCK_STATE_UNKNOWN)
    {
        /* As the GPS pulse triggers two times per second (because (in Linux) the interrupt triggers when the signal changes from high
         * to low and from low to high and we cannot seperate the two) we use "m_gpsUpdated" so that the GPS state only changes if there
         * wasn't any new GPS U-blox message after two pulses. */
        if(gpsData->ppsErrors < maxErrors)
        {
            gpsData->ppsErrors++;
            clockState = fallback ? GPS_CLOCK_STATE_INTERNAL : GPS_CLOCK_STATE_AVAILABLE;
        }
    }
    else
        gpsData->ppsErrors = 0;

    return clockState;
}

GpsClockState GPSClock::CalculateGpsToDeviceRate(bool fallback)
{
    uint64_t sysTimeOffset;
    GpsClockState clockState = GPS_CLOCK_STATE_UNKNOWN;
    GPSSyncData* gpsData = fallback ? &m_gpsFallbackData : &m_gpsData;
    GPSSyncData* gpsDataPrevious = fallback ? &m_gpsFallbackDataPrevious : &m_gpsDataPrevious;

    sysTimeOffset = gpsData->ppsSystemTime > gpsData->gpsSystemTime ?  gpsData->ppsSystemTime - gpsData->gpsSystemTime :
                                                                   gpsData->gpsSystemTime - gpsData->ppsSystemTime;

    if(gpsDataPrevious->gpsTime > 0 && gpsDataPrevious->ppsDeviceTime > 0)
    {
        if(sysTimeOffset <= NS_PER_SEC * 0.1)
        {
            if(gpsData->ppsDeviceTime - gpsDataPrevious->ppsDeviceTime > 0)
            {
                m_gpsToDeviceRate = (double)(gpsData->gpsTime - gpsDataPrevious->gpsTime) / (double)(gpsData->ppsDeviceTime - gpsDataPrevious->ppsDeviceTime);
                clockState = fallback ? GPS_CLOCK_STATE_INTERNAL : GPS_CLOCK_STATE_AVAILABLE;
            }
        }
        else
            ;//printf("ppsSystemTime: %lu; gpsSystemTime: %lu\n", gpsData->ppsSystemTime, gpsData->gpsSystemTime);
    }

    gpsDataPrevious->gpsTime = gpsData->gpsTime;
    gpsDataPrevious->gpsSystemTime = gpsData->gpsSystemTime;
    gpsDataPrevious->utcOffset = gpsData->utcOffset;

    return clockState;
}

bool GPSClock::GetGPSTime(uint64_t deviceTime, uint64_t* gpsTime, bool fallback)
{
    GPSSyncData* gpsData = fallback ? &m_gpsFallbackData : &m_gpsData;
    bool timeValid = false;

    pal::LockedRegionEnter(m_gpsLock);

    if(m_gpsToDeviceRate > 0)
    {
        uint64_t diffDeviceTime = deviceTime > gpsData->ppsDeviceTime ? deviceTime - gpsData->ppsDeviceTime : gpsData->ppsDeviceTime - deviceTime;
        *gpsTime = deviceTime > gpsData->ppsDeviceTime ? gpsData->gpsTime + m_gpsToDeviceRate * diffDeviceTime : gpsData->gpsTime - m_gpsToDeviceRate * diffDeviceTime;
        timeValid = true;//diffDeviceTime < maxDeviceTimeDiff * NS_PER_SEC;

        //printf("deviceTime: %lu\tgpsTime: %lu\tdiffDeviceTime: %lu\tm_gpsToDeviceRate: %f\n", deviceTime, *gpsTime, diffDeviceTime, m_gpsToDeviceRate);
    }

    pal::LockedRegionLeave(m_gpsLock);

    return timeValid;
}

uint16_t GPSClock::GetUtcOffset()
{
    return m_gpsData.utcOffset;
}

uint16_t GPSClock::GetFallbackUtcOffset()
{
    return m_gpsFallbackData.utcOffset;
}

double GPSClock::GetGpsToDeviceRate()
{
    return m_gpsToDeviceRate;
}

void GPSClock::SetPPSTimes(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool previous, bool fallback)
{
    GPSSyncData* gpsData = previous ? &m_gpsDataPrevious : &m_gpsData;
    GPSSyncData* gpsFallbackData = previous ? &m_gpsFallbackDataPrevious : &m_gpsFallbackData;

    if(!fallback)
    {
        gpsData->ppsDeviceTime = ppsDeviceTime;
        gpsData->ppsSystemTime = ppsSystemTime;
    }

    /* Always update the fallback times (NMEA messages) as these need to be up to date if
     * GPS is not available. */
    gpsFallbackData->ppsDeviceTime = ppsDeviceTime;
    gpsFallbackData->ppsSystemTime = ppsSystemTime;
}
