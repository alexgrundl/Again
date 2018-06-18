#include "gpssync.h"

GPSSync::GPSSync(Serial* serial, TimeAwareSystem* timeAwareSystem)
{
    m_serial = serial;
    m_gpsState = GPS_stat_CONFIG;
    m_gpsUbloxConfig = GPS_cfg_UNINIT;
    m_gpsChecksumErrorCounter = 0;
    m_CurrentUtcOffset = 0;
    m_timeAwareSystem = timeAwareSystem;
//    m_readTime = 0;

    m_syncThread = new CThreadWrapper<GPSSync>(this, &GPSSync::DoGpsSync, std::string("GPS Sync Thread"));
}

//void GPSSync::SetReadTime(uint64_t(*readTime)())
//{
//    m_readTime = readTime;
//}

void GPSSync::StartSync()
{
    m_syncThread->Start();
}

void GPSSync::StopSync()
{
    m_syncThread->Stop();
}

uint32_t GPSSync::DoGpsSync(bool_t* pbIsRunning, pal::EventHandle_t /*pWaitHandle*/)
{
  int type;
  char data[1024] = {0};
  uint32_t bytesRead = 0;

  //UART.SetWaitingCommMask();

    while (*pbIsRunning)
    {
        memset(data, 0, sizeof(data));
        bytesRead = 0;

        type = ReadMessage(data, &bytesRead);

        if (type >= 0)
        {
            if (m_gpsState == GPS_stat_READY)
            {
                if (type == 0)
                {
                  HandleData((uint8_t*)data, bytesRead);
                  //lognotice("GPS: NMEA info received");
                }
                else if (type == 2)
                {
                  HandlePPS((uint8_t*)data, bytesRead);
                  //lognotice("GPS: PPS info received %x %x %x %x %x %x, %u", data[0], data[1], data[2], data[3], data[4], data[5], bytesRead);
                }
            }
            else if (m_gpsState == GPS_stat_CONFIG)
            {
                if (type == 1)
                {
                  HandleUbloxConfigurationAck(1);
                }
                else if (type == 4)
                {
                  HandleUbloxConfigurationAck(0);
                }

                HandleUbloxConfiguration_tx();

                if (m_gpsUbloxConfig == GPS_cfg_DONE)
                {
                    m_gpsState = GPS_stat_READY; /* should be set after Configuration is performed */
                    //UART.SetBlockingState(true);
                }
            }
        }
        else
        {
            if ((type == -1) || (type == -2))
            {
                pal::ms_sleep(1000);
            }
        }
    }
    return 0;
}

void GPSSync::HandleUbloxConfiguration_tx()
{
  bool writeOk;
  static int errCounter = 0;

  switch (m_gpsUbloxConfig)
  {
  case(GPS_cfgNok_ubxCfgNmea) :
  case(GPS_cfg_UNINIT) :
    //XPTPD_LOG("START");
    writeOk = m_serial->Write((uint8_t*)GPS_ubxCfgNmea, sizeof(GPS_ubxCfgNmea));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfg_UNINIT;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(1, GPS_ubxCfgNmea, sizeof(GPS_ubxCfgNmea));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgNmea;
    }
    break;
  case(GPS_cfgNok_ubxCfgGprmc) :
  case(GPS_cfgok_ubxCfgNmea) :
    writeOk = m_serial->Write(GPS_ubxCfgGprmc, sizeof(GPS_ubxCfgGprmc));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgNmea;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(2, GPS_ubxCfgGprmc, sizeof(GPS_ubxCfgGprmc));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGprmc;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpvtg) :
  case(GPS_cfgok_ubxCfgGprmc) :
    writeOk = m_serial->Write(GPS_ubxCfgGpvtg, sizeof(GPS_ubxCfgGpvtg));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGprmc;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(3, GPS_ubxCfgGpvtg, sizeof(GPS_ubxCfgGpvtg));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpvtg;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpgga) :
  case(GPS_cfgok_ubxCfgGpvtg) :
    writeOk = m_serial->Write(GPS_ubxCfgGpgga, sizeof(GPS_ubxCfgGpgga));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpvtg;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(4, GPS_ubxCfgGpgga, sizeof(GPS_ubxCfgGpgga));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpgga;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpgsa) :
  case(GPS_cfgok_ubxCfgGpgga) :
    writeOk = m_serial->Write(GPS_ubxCfgGpgsa, sizeof(GPS_ubxCfgGpgsa));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpgga;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(5, GPS_ubxCfgGpgsa, sizeof(GPS_ubxCfgGpgsa));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpgsa;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpgsv) :
  case(GPS_cfgok_ubxCfgGpgsa) :
    writeOk = m_serial->Write(GPS_ubxCfgGpgsv, sizeof(GPS_ubxCfgGpgsv));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpgsa;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(6, GPS_ubxCfgGpgsv, sizeof(GPS_ubxCfgGpgsv));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpgsv;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpgll) :
  case(GPS_cfgok_ubxCfgGpgsv) :
    writeOk = m_serial->Write(GPS_ubxCfgGpgll, sizeof(GPS_ubxCfgGpgll));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpgsv;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(7, GPS_ubxCfgGpgll, sizeof(GPS_ubxCfgGpgll));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpgll;
    }
    break;
  case(GPS_cfgNok_ubxCfgGpzda) :
  case(GPS_cfgok_ubxCfgGpgll) :
    writeOk = m_serial->Write(GPS_ubxCfgGpzda, sizeof(GPS_ubxCfgGpzda));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpgll;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(8, GPS_ubxCfgGpzda, sizeof(GPS_ubxCfgGpzda));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgGpzda;
    }
    break;
  case(GPS_cfgNok_ubxCfgTp) :
  case(GPS_cfgok_ubxCfgGpzda) :
    writeOk = m_serial->Write(GPS_ubxCfgTp, sizeof(GPS_ubxCfgTp));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgGpzda;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(9, GPS_ubxCfgTp, sizeof(GPS_ubxCfgTp));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgTp;
    }
    break;
  case(GPS_cfgNok_ubxCfgMsgTp) :
  case(GPS_cfgok_ubxCfgTp) :
    writeOk = m_serial->Write(GPS_ubxCfgMsgTp, sizeof(GPS_ubxCfgMsgTp));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgTp;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(10, GPS_ubxCfgMsgTp, sizeof(GPS_ubxCfgMsgTp));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgMsgTp;
    }
    break;
  case(GPS_cfgNok_ubxCfgMsgUbx4) :
  case(GPS_cfgok_ubxCfgMsgTp) :
    writeOk = m_serial->Write(GPS_ubxCfgMsgUbx4, sizeof(GPS_ubxCfgMsgUbx4));
    if (writeOk)
    {
      m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgMsgTp;
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationSendInfo(11, GPS_ubxCfgMsgUbx4, sizeof(GPS_ubxCfgMsgUbx4));
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = GPS_cfg_ubxCfgMsgUbx4;
    }
    break;

  case(GPS_cfgok_ubxCfgMsgUbx4) :
    errCounter = 0;
    m_gpsUbloxConfigLast = GPS_cfgok_ubxCfgMsgUbx4;
    m_gpsUbloxConfig = GPS_cfg_DONE;
    //XPTPD_LOG("ENDE");
    break;
//******************************************************
  default:
    /* DET: unknown state occured */
    if (++errCounter > 100)
    {
#ifdef ACTIVATE_GPS_OUT
#define AGAIN_TXT "GPS: ******* SEND REQUEST AGAIN, BECAUSE NO ANSWER WAS RECEIVED IN TIME *******"
      fprintf(stdout, "GPS: *** --> UBX (Size %3d) *** %s\r\n", sizeof(AGAIN_TXT), AGAIN_TXT); fflush(stdout);
      XPTPD_LOG("GPS: *** --> UBX (Size %3d) *** %s", sizeof(AGAIN_TXT), AGAIN_TXT);
#endif //ACTIVATE_GPS_OUT
      errCounter = 0;
      m_gpsUbloxConfig = m_gpsUbloxConfigLast;
    }
    break;
  }
}

int GPSSync::ReadMessage(char* buffer, uint32_t* bytesRead)
{
  int ubclass = -1;
  int length = 0;
  int ubtype = 0;

  char szBuf = 0;
  char szBufOld = 0;
  int iUbxCounter = 0;
  bool bFrameComplete = false;


  #define CHECK_FRAME_TYPE          0
  #define RECV_NMEA_FRAME           1
  #define RECV_UBX_FRAME_PART_1     2
  #define RECV_UBX_FRAME_PART_2     3

  int RX_STATE = CHECK_FRAME_TYPE;

  *bytesRead = 0;
  do
  {
    szBufOld = szBuf;
    if (m_serial->ReadChar(buffer, *bytesRead))
    {
        szBuf = buffer[*bytesRead];
        *bytesRead+=1;
    }
    else
    {
      logerror("ReadFile failed");
    }

    switch(RX_STATE)
    {
      case CHECK_FRAME_TYPE:
      {
        if (szBuf == '$')
        {
          ubclass = 0;
          RX_STATE = RECV_NMEA_FRAME;
        }
        else if ((uint8_t)szBuf == 0xB5)//'µ')
        {
          iUbxCounter = 0;
          RX_STATE = RECV_UBX_FRAME_PART_1;
        }
        else
        {

          *bytesRead = 0;
          szBuf = 0;
          continue;
        }
      }
      break;

      case RECV_NMEA_FRAME:
      {
        if (*bytesRead > 500)
        {
          return -5;
        }

        if (szBufOld == '\r')
        {
          if (szBuf == '\n')
          {
            bFrameComplete = true;
          }
        }
      }
      break;

      case RECV_UBX_FRAME_PART_1:
      {
        if (iUbxCounter < 5)
        {
          switch (iUbxCounter)
          {
            case 0:
            {
              if (szBuf != 'b')
              {
                return -100;
              }
              break;
            }
            case 1:
            {
              ubclass = szBuf;
              break;
            }
            case 2:
            {
              ubtype = szBuf;
              break;
            }
            case 3:
            {
              length = szBuf;
              break;
            }
            case 4:
            {
              length += szBuf * 256;
              break;
            }
          }
          iUbxCounter++;

          if (iUbxCounter == 5)
          {
            iUbxCounter = 0;
            RX_STATE = RECV_UBX_FRAME_PART_2;
          }
        }
      }
      break;

      case RECV_UBX_FRAME_PART_2:
      {
        if (iUbxCounter >= (length + 2) - 1)
        {
          bFrameComplete = true;
        }
        iUbxCounter++;
      }
      break;

      default:
      break;
    }

  } while(bFrameComplete == false);

  if (ubclass == 0) //NMEA Data Message
  {
    return 0;
  }
  else if (ubclass == 0x05) //Acknowledge Message
  {
    if (ubtype == 1) //Acknowledged
    {
      return 1;
    }
    else //Not Acknowledged
    {
      return 4;
    }
  }
  else if (ubclass == 0x0D) //Time Pulse Message
  {
    return 2;
  }
  else
  {
    return -10;
  }
}

bool GPSSync::EvaluateChecksum(uint8_t const * const pData, uint16_t length)
{
  bool RetVal = FALSE;
  uint16_t i;
  bool CheckSumCalculated = FALSE;
  uint8_t checksum = 0;
  uint8_t CmpVal = 0;

  for (i = 0; i < length; i++)
  {
    if (!CheckSumCalculated)
    {
      if (i == 0 && pData[i] == '$')
      {
        checksum = 0;
      }
      else if (pData[i] == '*' && pData[i + 3] == '\r' && pData[i + 4] == '\n')
      {
        CheckSumCalculated = TRUE;
        if (pData[i + 1] >= '0' && pData[i + 1] <= '9')
        {
          CmpVal = 16 * (pData[i + 1] - '0');
        }
        else
        {
          CmpVal = 16 * (pData[i + 1] - '7');
        }

        if (pData[i + 2] >= '0' && pData[i + 2] <= '9')
        {
          CmpVal += (pData[i + 2] - '0');
        }
        else
        {
          CmpVal += (pData[i + 2] - '7');
        }

        if (CmpVal == checksum)
        {
          RetVal = TRUE;
        }

      }
      else
      {
        checksum ^= pData[i];
      }
    }
  }
  return RetVal;
}

void GPSSync::HandleData(uint8_t const * pData, uint16_t length)
{
  uint16_t  l;
  uint16_t  StringStart = 0;
  uint16_t  StringEnd = 0;
  uint8_t*  pBuffer;
  uint16_t len = 0;
  //GPSAvail_e  GPSavail = GPS_stat_NOTAVAIL;

  uint8_t * pGPRMCData = NULL;
  uint32_t GPRMSlength = 0;
  uint8_t  GPRMCfound = 0;

  GPSMsgID_e GpsTmp;
  bool CheckSumValid = FALSE;

  pBuffer = (uint8_t *)pData;

  for (l = 0; l < length; l++)
  {
    /* search for the first '$' indicating NMEA sentence start */
    if ((StringStart == 0) && pData[l] == '$')
    {
      StringStart = 1;
      StringEnd = 0;
      pBuffer = (uint8_t *)&pData[l];
    }

    /* search for valid NMEA sentence framing '$' [sentencedata] '\r\n' */
    if ((StringStart == 1) && (pData[l - 1] == '\r') && (pData[l] == '\n'))
    {
      /* toggle the buffer and prepare for new receiving */
      StringEnd = 1;
      len = (uint16_t)(&pData[l] - pBuffer);

      // Enforce  a maximal length.
      if (len > length)
      {
        len = length;
      }
      else
      {
      }

      CheckSumValid = EvaluateChecksum(pBuffer, (uint16_t)len);

      if (!CheckSumValid)
      {
        m_gpsChecksumErrorCounter++;
        StringStart = 0;
      }
      else
      {
        /* check what gps string type the last buffer contains */
        GpsTmp = ParseDataString(pBuffer, len);

        if (GpsTmp == GPRMC)
        {
          /* store last GPRMC Position */
          pGPRMCData = pBuffer;
          GPRMSlength = len;
          GPRMCfound++;
          //lognotice("GPS: Received GxRMC message");

          /* continue searching */
          StringStart = 0;

        }
        else if (GpsTmp == PUBX)
        {
          //Get leap seconds
          int position = 0;
          int leappositionstart = 0;
          int leappositionstop = 0;
          for (uint32_t i = 0; i < len; i++)
          {
            if (pBuffer[i] == ',')
            {
              position++;
              if (position == 6)
                leappositionstart = i;
              if (position == 7)
                leappositionstop = i;
            }
          }
          if (leappositionstop - leappositionstart > 3)
          {
            if (pBuffer[leappositionstop - 1] == 'D')
            {
              //default leap seconds no GPS
              uint16_t leap;
              leap = pBuffer[leappositionstart + 1] - '0';
              leap = leap * 10 + (pBuffer[leappositionstart + 2] - '0');
              m_CurrentUtcOffset = leap + 19;
              //printf("currentUtcOffset: %u\n", m_CurrentUtcOffset);
            }
            else
              return;
          }
          else
          {
            uint16_t leap;
            leap = pBuffer[leappositionstart + 1] - '0';
            leap = leap * 10 + (pBuffer[leappositionstart + 2] - '0');
            m_CurrentUtcOffset = leap + 19;
            //printf("currentUtcOffset: %u\n", m_CurrentUtcOffset);
          }
        }
        else
        {
          /* continue searching */
          StringStart = 0;
        }
      }
    }
    else if ((StringStart == 1) && (StringEnd == 0))
    {
      /* ignore this char, evaluate next char */
    }
  }

  /* evaluate GPRMC, if found */
  if (GPRMCfound == 1)
  {
    uint64_t gps_time = 0;
    uint64_t tsc_time = 0;
    int iRetVal = ExtractTimestamp(pGPRMCData, GPRMSlength, &gps_time, &tsc_time);
    if (iRetVal == 0)
    {
      gps_time = gps_time + ((uint64_t)(m_CurrentUtcOffset)* 1000000000); //add current_UTC_TAI_offset
    }
    //XPTPD_LOG("GPS: Received GxRMC message --> GPS_handleFallbackPPS");
    HandleFallbackPPS(gps_time, tsc_time);
  }
  else if (GPRMCfound > 1)
  {
    /* several GPRMC found  - ignore them  */
  }
  else
  {
    /* no GPRMC found */
  }
}

GPSMsgID_e GPSSync::ParseDataString(uint8_t const * pStrData, uint32_t length)
{
  GPSMsgID_e RetVal = Invalid;
  uint16_t i;
  uint16_t len;

  /* check all possible ids for a match */
  if ((length >= 7) && (pStrData[0] == '$'))
  {
    if ((pStrData[1] == 'G'))
    {
      /* default NMEA sentence */
      len = sizeof(NMEAIdentification) / sizeof(GPSMsgIDstring_t);
      for (i = 0; i < len; i++)
      {
        if ((pStrData[3] == NMEAIdentification[i].IDS[0]) &&
          (pStrData[4] == NMEAIdentification[i].IDS[1]) &&
          (pStrData[5] == NMEAIdentification[i].IDS[2]) &&
          (pStrData[6] == ','))
        {
          RetVal = (GPSMsgID_e)NMEAIdentification[i].EnumID;
          /* abort on match */
          i = len;
        }
      }
    }
    else if ((pStrData[1] == 'P') && (pStrData[2] == 'U'))
    {
      RetVal = PUBX;
    }
  }
  else
  {
    /* DET: String to short or does not start with '$' */
  }
  return RetVal;
}

int GPSSync::ExtractTimestamp(uint8_t const * const pData, uint32_t length, uint64_t *time, uint64_t *tsc_time)
{
    uint16_t i;
    static GPS_gprmc_t RMCValid;
    GPSTime_t   GPSTime;
    uint16_t  tYear;

//    if(m_readTime != NULL)
//        *tsc_time = m_readTime();
    *tsc_time = m_timeAwareSystem->ReadCurrentTime().ns;

    if ((void *)pData == (void *)0)
    {
        return -1;
    }

    /* check if enough colons are in the sentence */
    RMCValid.Validness.All = 0;
    for (i = 0; i <= length; i++)
    {
        if (pData[i] == ',')
        {
            RMCValid.Validness.All |= 1 << RMCValid.Validness.ValidField.IndexCnt;
            RMCValid.Position[RMCValid.Validness.ValidField.IndexCnt] = i;
            RMCValid.Validness.ValidField.IndexCnt++;
        }
    }

    /* all expected fields seem to be available */
    if (RMCValid.Position[1] - RMCValid.Position[0] >= 7)
    {
        /* the time contains at least a timestamp */
        GPSTime.Hour = (10 * (pData[RMCValid.Position[0] + 1] - '0')) + ((pData[RMCValid.Position[0] + 2] - '0'));
        GPSTime.Min = (10 * (pData[RMCValid.Position[0] + 3] - '0')) + ((pData[RMCValid.Position[0] + 4] - '0'));
        GPSTime.Sec = (10 * (pData[RMCValid.Position[0] + 5] - '0')) + ((pData[RMCValid.Position[0] + 6] - '0'));

        /* check for second fragments */
        if (RMCValid.Position[1] - RMCValid.Position[0] == 11)
        {
            GPSTime.mSec = (100 * (pData[RMCValid.Position[0] + 8] - '0')) +
            (10 * (pData[RMCValid.Position[0] + 9] - '0')) +
            ((pData[RMCValid.Position[0] + 10] - '0'));
        }
        else
        {
            GPSTime.mSec = 0;
        }

        /* check for date */
        if (RMCValid.Position[9] - RMCValid.Position[8] == 7)
        {
            /* a datestamp is also available */
            GPSTime.Day = (10 * (pData[RMCValid.Position[8] + 1] - '0')) + ((pData[RMCValid.Position[8] + 2] - '0'));
            GPSTime.Month = (10 * (pData[RMCValid.Position[8] + 3] - '0')) + ((pData[RMCValid.Position[8] + 4] - '0'));

            /* get the year, treat everything between 0 and 90 as 2000 to 2090, all other as 1991 to 1999 */
            tYear = (10 * (pData[RMCValid.Position[8] + 5] - '0')) + ((pData[RMCValid.Position[8] + 6] - '0'));

            if (tYear <= 90)
            {
            tYear += 2000;
            }
            else
            {
            tYear += 1900;
            }
            GPSTime.Year = tYear;

            /* get the timestamp from 1970 (unix) by passing the received UTC time */
            CalcUnixTimestamp((uint64_t *)time, &GPSTime, 0);

            //XPTPD_LOG("GPS: Received GPSTime %d.%d.%d - %d:%d:%d.%d --> UnixTime %lld ns", GPSTime.Day, GPSTime.Month, GPSTime.Year, GPSTime.Hour, GPSTime.Min, GPSTime.Sec, GPSTime.mSec, *time);
        }
        return 0;
    }

    return -2;
}


void GPSSync::CalcUnixTimestamp(uint64_t * const UnixTimestamp, GPSTime_t * const gpsTime, uint16_t microseconds)
{
  int32_t j;
  int32_t u;
  uint64_t GPS_unixUsecs;

  GPS_unixUsecs = 0;
  /* Fix the warning missing suggest parentheses at if condition CBA 3.12.2015*/
  for (j = 1970; j < (gpsTime->Year); j++)
  {
    if ((((j % 4) == 0) && ((j % 100) != 0)) || ((j % 400) == 0))
    {
      //366days
      // unix_microsecs +=(366*24*60*60*1000000)  0x00001CC2A9EB4000;
      GPS_unixUsecs += (uint64_t)0x00001CC2A9EB4000;
    }
    else
    {
      //365days
      // unix_microsecs +=(365*24*60*60*1000000)  0x00001CAE8C13E000;
      GPS_unixUsecs += (uint64_t)0x00001CAE8C13E000;
    }
  }
  for (u = 1; u < (gpsTime->Month); u++)                //monatstage 31/30/29/28
  {
    if (u == 1 || u == 3 || u == 5 || u == 7 || u == 8 || u == 10 || u == 12)
    {
      // 31days
      // unix_microsecs +=(31*24*60*60*1000000)   0x0000026F9D14A000;
      GPS_unixUsecs += (uint64_t)0x0000026F9D14A000;
    }   //31 tägige monate
    if (u == 4 || u == 6 || u == 9 || u == 11)
    {
      // 30days
      // unix_microsecs +=(30*24*60*60*1000000)   0x0000025B7F3D4000;
      GPS_unixUsecs += (uint64_t)0x0000025B7F3D4000;
    }
    /* Fix the warning missing suggest parentheses at if condition CBA 3.12.2015*/
    if (u == 2)
    {
      if (((j % 4 == 0) && (j % 100 != 0)) || (j % 400 == 0))
      {
        // february with 29 days
        // unix_microsecs +=(29*24*60*60*1000000)   0x000002476165E000;
        GPS_unixUsecs += (uint64_t)0x000002476165E000;
      }
      else
      {
        // february with 28 days
        // unix_microsecs +=(28*24*60*60*1000000)   0x00000233438E8000;
        GPS_unixUsecs += (uint64_t)0x00000233438E8000;
      }
    }
  }

  // unix_microsecs +=((day-1)*24*60*60*1000000)   0x000000141DD76000;
  GPS_unixUsecs += (uint64_t)0x000000141DD76000 * ((gpsTime->Day) - 1);
  // unix_microsecs +=(hour * 60*60*1000000)   0x00000000D693A400;
  GPS_unixUsecs += (uint64_t)0x00000000D693A400 * (gpsTime->Hour);

  // unix_microsecs +=(minute*60*1000000)  0x3938700uL;
  GPS_unixUsecs += (uint64_t)0x3938700uL * (gpsTime->Min);
  // unix_microsecs +=(second *1000000) 0x000F4240uL;
  GPS_unixUsecs += (uint64_t)0x000F4240uL * (gpsTime->Sec);
  // unix_microsecs +=(milisecond *1000)   0x000003E8uL;
  GPS_unixUsecs += (uint64_t)0x3E8uL * (gpsTime->mSec);
  GPS_unixUsecs += microseconds;

  *UnixTimestamp = GPS_unixUsecs * 1000;
}

void GPSSync::HandleFallbackPPS(uint64_t gps_time, uint64_t tsc_time)
{
  //lognotice("GPS: Set new Fbk Time: %lu: %lu: %u", gps_time, tsc_time, m_CurrentUtcOffset);

  m_timeAwareSystem->SetLastFallbackGpsData(GPSSyncData({gps_time, tsc_time, m_CurrentUtcOffset}));
  //TimeControl->SetLastGpsFallbackData(gps_time, tsc_time, current_utc_offset);
}

void GPSSync::HandleUbloxConfigurationAck(uint8_t ack)
{
    switch (m_gpsUbloxConfig)
    {
    case(GPS_cfg_UNINIT) :
      /* DET: Chars transmitted without config state */
      break;
    case(GPS_cfg_ubxCfgNmea) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgNmea; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgNmea; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(1, ack, GPS_ubxCfgNmea, sizeof(GPS_ubxCfgNmea));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGprmc) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGprmc; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGprmc; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(2, ack, GPS_ubxCfgGprmc, sizeof(GPS_ubxCfgGprmc));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpvtg) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpvtg; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpvtg; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(3, ack, GPS_ubxCfgGpvtg, sizeof(GPS_ubxCfgGpvtg));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpgga) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpgga; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpgga; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(4, ack, GPS_ubxCfgGpgga, sizeof(GPS_ubxCfgGpgga));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpgsa) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpgsa; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpgsa; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(5, ack, GPS_ubxCfgGpgsa, sizeof(GPS_ubxCfgGpgsa));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpgsv) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpgsv; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpgsv; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(6, ack, GPS_ubxCfgGpgsv, sizeof(GPS_ubxCfgGpgsv));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpgll) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpgll; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpgll; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(7, ack, GPS_ubxCfgGpgll, sizeof(GPS_ubxCfgGpgll));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgGpzda) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgGpzda; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgGpzda; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(8, ack, GPS_ubxCfgGpzda, sizeof(GPS_ubxCfgGpzda));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgTp) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgTp; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgTp; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(9, ack, GPS_ubxCfgTp, sizeof(GPS_ubxCfgTp));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgMsgTp) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgMsgTp; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgMsgTp; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(10, ack, GPS_ubxCfgMsgTp, sizeof(GPS_ubxCfgMsgTp));
#endif //ACTIVATE_GPS_OUT
      break;
    case(GPS_cfg_ubxCfgMsgUbx4) :
      if (ack > 0) { m_gpsUbloxConfig = GPS_cfgok_ubxCfgMsgUbx4; }
      else { m_gpsUbloxConfig = GPS_cfgNok_ubxCfgMsgUbx4; }
#ifdef ACTIVATE_GPS_OUT
      GPS_UbloxConfigurationRecvInfo(11, ack, GPS_ubxCfgMsgUbx4, sizeof(GPS_ubxCfgMsgUbx4));
#endif //ACTIVATE_GPS_OUT
      break;
    //case(GPS_cfg_ubxCfgTp5) :
    //  if (ack > 0) { GPS_ublox_config = GPS_cfgok_ubxCfgTp5; }
    //  else { GPS_ublox_config = GPS_cfgNok_ubxCfgTp5; }
    //  break;
    default:
      /* DET: unexpected state */
      break;
    }
}

void GPSSync::HandlePPS(uint8_t const * pData, uint16_t length)
{
    length = length;

    if (pData[0] == 0xB5 && pData[1] == 0x62 && pData[2] == 0x0D && pData[3] == 0x01)
    {
        uint64_t current_ts = 0;
//        if(m_readTime != 0)
//            current_ts = m_readTime();
        current_ts = m_timeAwareSystem->ReadCurrentTime().ns;

        uint32_t towms = pData[9];
        towms = (towms << 8) + pData[8];
        towms = (towms << 8) + pData[7];
        towms = (towms << 8) + pData[6];

        uint32_t towsms = pData[13];
        towsms = (towsms << 8) + pData[12];
        towsms = (towsms << 8) + pData[11];
        towsms = (towsms << 8) + pData[10];

        int32_t qerr = pData[17];
        qerr = (qerr << 8) + pData[16];
        qerr = (qerr << 8) + pData[15];
        qerr = (qerr << 8) + pData[14];

        uint16_t week = pData[19];
        week = (week << 8) + pData[18];

        //bool timbaseutc = pData[20] & 0x01;

        uint64_t weekmseconds = (uint64_t)week * 604800000;

        weekmseconds += towms;

        uint64_t subms = towsms;
        subms = (subms * 15625) / 67108864;
        uint64_t gpsns = weekmseconds * 1000000 + subms;

        gpsns = gpsns + qerr / 1000; // Correction of quantization error of timepulse qerr in ps

        uint64_t tains = gpsns + 315964818000000000;  //315964819000000000 - 1000000000 = TAI-GPS in ns

        //lognotice("GPS: Set new GPS Time: %lu: %lu: %u", tains, current_ts, m_CurrentUtcOffset);

        m_timeAwareSystem->SetLastGpsData(GPSSyncData({tains, current_ts, m_CurrentUtcOffset}));
        //TimeControl->SetLastGpsData(tains, tsctime.GetTime(), current_utc_offset);
    }
    else
    {
        logerror("GPS: Error receiving GPS PPS info");
    }

}
