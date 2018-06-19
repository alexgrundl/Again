#include "gpsclock.h"

GPSClock::GPSClock()
{
    m_gpsLock = pal::LockedRegionCreate();
    m_GpsData = {0, 0, 0, 0, 0, false};
    m_GpsDataPrevious = {0, 0, 0, 0, 0, false};
    m_GpsFallbackData = {0, 0, 0, 0, 0, false};
    m_gpsToDeviceRate = 0.0;
}

GPSClock::~GPSClock()
{
    pal::LockedRegionDelete(m_gpsLock);
}

void GPSClock::UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset)
{
    pal::LockedRegionEnter(m_gpsLock);
    m_GpsDataPrevious.gpsTime = m_GpsData.gpsTime;
    m_GpsDataPrevious.gpsSystemTime = m_GpsData.gpsSystemTime;
    m_GpsDataPrevious.utcOffset = m_GpsData.utcOffset;

    m_GpsData.gpsTime = gpsTime;
    m_GpsData.gpsSystemTime = gpsSystemTime;
    m_GpsData.utcOffset = utcOffset;

    m_GpsData.gpsTimeUpdated = true;
    pal::LockedRegionLeave(m_gpsLock);
}

void GPSClock::UpdateFallbackGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset)
{
    pal::LockedRegionEnter(m_gpsLock);
    m_GpsFallbackData.gpsTime = gpsTime;
    m_GpsFallbackData.gpsSystemTime = gpsSystemTime;
    m_GpsFallbackData.utcOffset = utcOffset;
    pal::LockedRegionLeave(m_gpsLock);
}

bool GPSClock::UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime)
{
    int nWaits = 0;
    int maxWaits = 5;
    uint64_t sysTimeOffset;
    bool timeAndFrequencySet = false;

    while(!m_GpsData.gpsTimeUpdated)
    {
        pal::ms_sleep(20);
        nWaits++;
        if(nWaits >= maxWaits)
            break;
    }

    pal::LockedRegionEnter(m_gpsLock);
    if(m_GpsData.gpsTimeUpdated)
    {
        m_GpsData.gpsTimeUpdated = false;
        sysTimeOffset = ppsSystemTime > m_GpsData.gpsSystemTime ?  ppsSystemTime - m_GpsData.gpsSystemTime :
                                                                       m_GpsData.gpsSystemTime - ppsSystemTime;

        if(sysTimeOffset <= NS_PER_SEC * 0.1)
        {
            m_GpsData.ppsDeviceTime = ppsDeviceTime;
            m_GpsData.ppsSystemTime = ppsSystemTime;

            if(m_GpsDataPrevious.gpsTime > 0 && m_GpsDataPrevious.ppsDeviceTime > 0 && m_GpsData.ppsDeviceTime - m_GpsDataPrevious.ppsDeviceTime > 0)
            {
                m_gpsToDeviceRate = (double)(m_GpsData.gpsTime - m_GpsDataPrevious.gpsTime) / (double)(m_GpsData.ppsDeviceTime - m_GpsDataPrevious.ppsDeviceTime);
                timeAndFrequencySet = true;
            }

            m_GpsDataPrevious.ppsDeviceTime = m_GpsData.ppsDeviceTime;
            m_GpsDataPrevious.ppsSystemTime = m_GpsData.ppsSystemTime;
        }
        else
        {
            logerror("UpdateGPSDataFromPPS: Wrong sysTimeOffset: %lu", sysTimeOffset);
        }
    }
    pal::LockedRegionLeave(m_gpsLock);

    return timeAndFrequencySet;
}

bool GPSClock::GetGPSTime(uint64_t deviceTime, uint64_t* gpsTime)
{
    bool timeValid = false;

    pal::LockedRegionEnter(m_gpsLock);

    if(m_gpsToDeviceRate > 0)
    {
        uint64_t diffDeviceTime = deviceTime > m_GpsData.ppsDeviceTime ? deviceTime - m_GpsData.ppsDeviceTime : m_GpsData.ppsDeviceTime - deviceTime;
        *gpsTime = deviceTime > m_GpsData.ppsDeviceTime ? m_GpsData.gpsTime + m_gpsToDeviceRate * diffDeviceTime : m_GpsData.gpsTime - m_gpsToDeviceRate * diffDeviceTime;
        timeValid = diffDeviceTime < maxDeviceTimeDiff * NS_PER_SEC;

        //printf("deviceTime: %lu\tgpsTime: %lu\tdiffDeviceTime: %lu\tm_gpsToDeviceRate: %f\n", deviceTime, *gpsTime, diffDeviceTime, m_gpsToDeviceRate);
    }

    pal::LockedRegionLeave(m_gpsLock);

    return timeValid;
}
