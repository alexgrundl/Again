#include "gpsclock.h"

GPSClock::GPSClock()
{
    m_gpsLock = pal::LockedRegionCreate();
    m_gpsDataNew = {0, 0, 0, 0, 0, 0};
    m_gpsData = {0, 0, 0, 0, 0, 0};
    m_gpsData.ppsErrors = k_maxGpsErrors;
    m_isFallback = false;
    m_gpsToDeviceRate = 0.0;
}

GPSClock::~GPSClock()
{
    pal::LockedRegionDelete(m_gpsLock);
}

void GPSClock::SetAsFallback()
{
    m_isFallback = true;
}

bool GPSClock::IsFallback()
{
    return m_isFallback;
}

uint32_t GPSClock::GetMaxErrors()
{
    return m_isFallback ? k_maxGpsFallbackErrors : k_maxGpsErrors;
}

void GPSClock::UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset)
{
    pal::LockedRegionEnter(m_gpsLock);

    m_gpsDataNew.gpsTime = gpsTime;
    m_gpsDataNew.gpsSystemTime = gpsSystemTime;
    m_gpsDataNew.utcOffset = utcOffset;

    pal::LockedRegionLeave(m_gpsLock);
}

GpsClockState GPSClock::UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime)
{
    int nWaits = 0;
    int maxWaits = 5;
    GpsClockState clockState;

    while(m_gpsDataNew.gpsSystemTime == m_gpsData.gpsSystemTime)
    {
        pal::ms_sleep(20);
        nWaits++;
        if(nWaits >= maxWaits)
            break;
    }

    if(m_gpsDataNew.gpsSystemTime == m_gpsData.gpsSystemTime)
    {
        if(m_gpsData.ppsErrors < GetMaxErrors())
        {
            m_gpsData.ppsErrors++;
            return m_lastState;
        }
        else
            return GPS_CLOCK_STATE_UNKNOWN;
    }

    pal::LockedRegionEnter(m_gpsLock);

    m_gpsData.ppsErrors = 0;
    SetPPSTimes(ppsDeviceTime, ppsSystemTime, false);
    clockState = CalculateGpsToDeviceRate();
    SetPPSTimes(ppsDeviceTime, ppsSystemTime, true);
    m_lastState = clockState;

    //printf("gpsTimePrev: %lu, gpsTime: %lu, ppsTime: %lu\n", m_gpsData.gpsTime, m_gpsDataNew.gpsTime, m_gpsDataNew.ppsDeviceTime);
    m_gpsData.gpsTime = m_gpsDataNew.gpsTime;
    m_gpsData.gpsSystemTime = m_gpsDataNew.gpsSystemTime;
    m_gpsData.utcOffset = m_gpsDataNew.utcOffset;

    pal::LockedRegionLeave(m_gpsLock);
    return clockState;
}

GpsClockState GPSClock::CalculateGpsToDeviceRate()
{
    uint64_t sysTimeOffset;
    GpsClockState clockState = GPS_CLOCK_STATE_UNKNOWN;

    sysTimeOffset = m_gpsData.ppsSystemTime > m_gpsData.gpsSystemTime ?  m_gpsData.ppsSystemTime - m_gpsData.gpsSystemTime :
                                                                   m_gpsData.gpsSystemTime - m_gpsData.ppsSystemTime;

    if(m_gpsData.gpsTime > 0 && m_gpsData.ppsDeviceTime > 0)
    {
        if(sysTimeOffset <= NS_PER_SEC * 0.1)
        {
            if(m_gpsDataNew.ppsDeviceTime - m_gpsData.ppsDeviceTime > 0)
            {
                m_gpsToDeviceRate = (double)(m_gpsDataNew.gpsTime - m_gpsData.gpsTime) / (double)(m_gpsDataNew.ppsDeviceTime - m_gpsData.ppsDeviceTime);
                clockState = m_isFallback ? GPS_CLOCK_STATE_INTERNAL : GPS_CLOCK_STATE_AVAILABLE;
            }
        }
        else
        {
            //printf("ppsSystemTime: %lu; gpsSystemTime: %lu\n", m_gpsData.ppsSystemTime, m_gpsData.gpsSystemTime);
        }
    }

    return clockState;
}

bool GPSClock::GetGPSTime(uint64_t deviceTime, uint64_t* gpsTime)
{
    bool timeValid = false;

    pal::LockedRegionEnter(m_gpsLock);

    if(m_gpsToDeviceRate > 0)
    {
        uint64_t diffDeviceTime = deviceTime > m_gpsData.ppsDeviceTime ? deviceTime - m_gpsData.ppsDeviceTime : m_gpsData.ppsDeviceTime - deviceTime;
        *gpsTime = deviceTime > m_gpsData.ppsDeviceTime ? m_gpsData.gpsTime + m_gpsToDeviceRate * diffDeviceTime : m_gpsData.gpsTime - m_gpsToDeviceRate * diffDeviceTime;
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

double GPSClock::GetGpsToDeviceRate()
{
    return m_gpsToDeviceRate;
}

void GPSClock::SetPPSTimes(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool current)
{
    GPSSyncData* gpsData = current ? &m_gpsData : &m_gpsDataNew;

    gpsData->ppsDeviceTime = ppsDeviceTime;
    gpsData->ppsSystemTime = ppsSystemTime;
}
