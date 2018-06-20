#ifndef GPSCLOCK_H
#define GPSCLOCK_H

#include "types.h"
#include "platform.h"
#include "GPSdef.h"

class GPSClock
{
public:


    GPSClock();


    virtual ~GPSClock();


    void UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset, bool fallback);


    GpsClockState UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime);


    bool GetGPSTime(uint64_t deviceTime, uint64_t *gpsTime, bool fallback);


    uint16_t GetUtcOffset();


    uint16_t GetFallbackUtcOffset();


    double GetGpsToDeviceRate();

private:

    static const int k_maxGpsErrors = 2;

    static const int k_maxGpsFallbackErrors = 10;

    /**
     * @brief maxGPSTimeDelay The max. difference between current and last device time when requesting the GPS time.
     * If the difference is higher than that value "GetGPSTime" will return false signaling that GPS data is too old.
     */
    static const uint32_t maxDeviceTimeDiff = 20;

    GPSSyncData m_gpsData;


    GPSSyncData m_gpsDataPrevious;


    GPSSyncData m_gpsFallbackData;


    GPSSyncData m_gpsFallbackDataPrevious;


    double m_gpsToDeviceRate;


    pal::SectionLock_t m_gpsLock;


    GpsClockState UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool fallback, uint32_t maxErrors);


    GpsClockState CalculateGpsToDeviceRate(bool fallback);


    void SetPPSTimes(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool previous, bool fallback);
};

#endif // GPSCLOCK_H
