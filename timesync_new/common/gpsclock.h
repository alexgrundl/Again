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


    void UpdateGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset);


    GpsClockState UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime);


    bool GetGPSTime(uint64_t deviceTime, uint64_t *gpsTime);


    uint16_t GetUtcOffset();


    double GetGpsToDeviceRate();


    bool IsFallback();

    void SetAsFallback();

private:

    static const uint32_t k_maxGpsErrors = 4;

    static const uint32_t k_maxGpsFallbackErrors = 10;

    /**
     * @brief maxGPSTimeDelay The max. difference between current and last device time when requesting the GPS time.
     * If the difference is higher than that value "GetGPSTime" will return false signaling that GPS data is too old.
     */
    static const uint32_t maxDeviceTimeDiff = 20;


    GPSSyncData m_gpsDataNew;


    GPSSyncData m_gpsData;


    pal::SectionLock_t m_gpsLock;


    GpsClockState m_lastState;


    bool m_isFallback;


    double m_gpsToDeviceRate;


    GpsClockState UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool fallback);


    GpsClockState CalculateGpsToDeviceRate();


    void SetPPSTimes(uint64_t ppsDeviceTime, uint64_t ppsSystemTime, bool current);


    uint32_t GetMaxErrors();
};

#endif // GPSCLOCK_H
