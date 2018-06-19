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


    void UpdateFallbackGpsData(uint64_t gpsTime, uint64_t gpsSystemTime, uint16_t utcOffset);


    bool UpdateGPSDataFromPPS(uint64_t ppsDeviceTime, uint64_t ppsSystemTime);


    bool GetGPSTime(uint64_t deviceTime, uint64_t *gpsTime);

private:

    /**
     * @brief maxGPSTimeDelay The max. difference between current and last device time when requesting the GPS time.
     * If the difference is higher than that value "GetGPSTime" will return false signaling that GPS data is too old.
     */
    static const uint32_t maxDeviceTimeDiff = 20;

    GPSSyncData m_GpsData;


    GPSSyncData m_GpsDataPrevious;


    GPSSyncData m_GpsFallbackData;


    double m_gpsToDeviceRate;


    pal::SectionLock_t m_gpsLock;
};

#endif // GPSCLOCK_H
