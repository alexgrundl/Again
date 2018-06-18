#ifndef GPSSYNC_H
#define GPSSYNC_H

#include "types.h"
#include "ThreadWrapper.h"
#include "serial.h"
#include "GPSdef.h"
#include "timeawaresystem.h"

class GPSSync
{
public:

    GPSSync(Serial* serial, TimeAwareSystem* timeAwareSystem);


    void StartSync();


    void StopSync();


//    void SetReadTime(uint64_t(*readTime)());

private:

    Serial* m_serial;


    GPS_hw_e m_gpsState;


    GPS_cgf_st_e m_gpsUbloxConfig;


    GPS_cgf_st_e  m_gpsUbloxConfigLast;


    uint32_t m_gpsChecksumErrorCounter;


    uint16_t m_CurrentUtcOffset;


    CThreadWrapper<GPSSync>* m_syncThread;


    TimeAwareSystem* m_timeAwareSystem;

//    uint64_t (*m_readTime)();


    uint32_t DoGpsSync(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle);


    int ReadMessage(char* buffer, uint32_t* bytesRead);


    void HandleData(uint8_t const * pData, uint16_t length);


    bool EvaluateChecksum(uint8_t const * const pData, uint16_t length);


    GPSMsgID_e ParseDataString(uint8_t const * pStrData, uint32_t length);


    int ExtractTimestamp(uint8_t const * const pData, uint32_t length, uint64_t *time, uint64_t *tsc_time);


    void CalcUnixTimestamp(uint64_t * const UnixTimestamp, GPSTime_t * const gpsTime, uint16_t microseconds);


    void HandleFallbackPPS(uint64_t gps_time, uint64_t tsc_time);


    void HandleUbloxConfiguration_tx();


    void HandleUbloxConfigurationAck(uint8_t ack);


    void HandlePPS(uint8_t const * pData, uint16_t length);
};

#endif // GPSSYNC_H
