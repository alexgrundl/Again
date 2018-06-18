#ifndef GPSDEF_H

#define GPSDEF_H

/** \brief GPS hardware availability
* Enumaration with GPS availabilty constants. 
*
* It is used to describe general GPS availability in the system. 
*/
typedef enum GPS_hw
{
  GPS_stat_UNINIT      = 3,    /*!< GPS UART interface is not initialized */
  GPS_stat_INIT        = 4,    /*!< GPS UART interface is initialized */
  GPS_stat_CONFIG      = 5,    /*!< GPS Module Configuration must be performed */
  GPS_stat_READY       = 6,    /*!< GPS Module Configuration has been performed */
} GPS_hw_e;


typedef enum GPS_cfg_st
{
  GPS_cfg_UNINIT  = 0x00, /**< Configuration has to be started. */ 
  GPS_cfg_ubxCfgNmea        = 0x01,   /*!< Send msg ubxCfgNmea */
  GPS_cfgok_ubxCfgNmea      = 0x02,   /*!< Message ubxCfgNmea has been acknowledged. */
  GPS_cfgNok_ubxCfgNmea     = 0x03,   /*!< Message ubxCfgNmea was not acknowledged. */
  GPS_cfg_ubxCfgGprmc       = 0x11,   /*!< Send msg ubxCfgGprmc */
  GPS_cfgok_ubxCfgGprmc     = 0x12,   /*!< Message ubxCfgGprmc has been acknowledged. */
  GPS_cfgNok_ubxCfgGprmc    = 0x13,   /*!< Message ubxCfgGprmc was not acknowledged. */
  GPS_cfg_ubxCfgGpvtg       = 0x21,   /*!< Send msg ubxCfgGpvtg */
  GPS_cfgok_ubxCfgGpvtg     = 0x22,   /*!< Message ubxCfgGpvtg has been acknowledged. */
  GPS_cfgNok_ubxCfgGpvtg    = 0x23,   /*!< Message ubxCfgGpvtg was not acknowledged. */
  GPS_cfg_ubxCfgGpgga       = 0x31,   /*!< Send msg ubxCfgGpgga */
  GPS_cfgok_ubxCfgGpgga     = 0x32,   /*!< Message ubxCfgGpgga has been acknowledged. */
  GPS_cfgNok_ubxCfgGpgga    = 0x33,   /*!< Message ubxCfgGpgga was not acknowledged. */
  GPS_cfg_ubxCfgGpgsa       = 0x41,   /*!< Send msg ubxCfgGpgga */
  GPS_cfgok_ubxCfgGpgsa     = 0x42,   /*!< Message ubxCfgGpgga has been acknowledged. */
  GPS_cfgNok_ubxCfgGpgsa    = 0x43,   /*!< Message ubxCfgGpgga was not acknowledged. */
  GPS_cfg_ubxCfgGpgsv       = 0x51,   /*!< Send msg ubxCfgGpgsv */
  GPS_cfgok_ubxCfgGpgsv     = 0x52,   /*!< Message ubxCfgGpgsv has been acknowledged. */
  GPS_cfgNok_ubxCfgGpgsv    = 0x53,   /*!< Message ubxCfgGpgsv was not acknowledged. */
  GPS_cfg_ubxCfgGpgll       = 0x61,   /*!< Send msg ubxCfgGpgll */
  GPS_cfgok_ubxCfgGpgll     = 0x62,   /*!< Message ubxCfgGpgll has been acknowledged. */
  GPS_cfgNok_ubxCfgGpgll    = 0x63,   /*!< Message ubxCfgGpgll was not acknowledged. */
  GPS_cfg_ubxCfgGpzda       = 0x71,   /*!< Send msg ubxCfgGpzda */
  GPS_cfgok_ubxCfgGpzda     = 0x72,   /*!< Message ubxCfgGpzda has been acknowledged. */
  GPS_cfgNok_ubxCfgGpzda    = 0x73,   /*!< Message ubxCfgGpzda was not acknowledged. */
  GPS_cfg_ubxCfgTp          = 0x81,   /*!< Send msg ubxCfgTp */
  GPS_cfgok_ubxCfgTp        = 0x82,   /*!< Message ubxCfgTp has been acknowledged. */
  GPS_cfgNok_ubxCfgTp       = 0x83,   /*!< Message ubxCfgTp was not acknowledged. */
  GPS_cfg_ubxCfgMsgTp       = 0x91,   /*!< Send msg ubxCfgMsgTp */
  GPS_cfgok_ubxCfgMsgTp     = 0x92,   /*!< Message ubxCfgMsgTp has been acknowledged. */
  GPS_cfgNok_ubxCfgMsgTp    = 0x93,   /*!< Message ubxCfgMsgTp was not acknowledged. */
  GPS_cfg_ubxCfgMsgUbx4     = 0xA1,   /*!< Send msg ubxCfgMsgUbx4 */
  GPS_cfgok_ubxCfgMsgUbx4   = 0xA2,   /*!< Message ubxCfgMsgUbx4 has been acknowledged. */
  GPS_cfgNok_ubxCfgMsgUbx4  = 0xA3,   /*!< Message ubxCfgMsgUbx4 was not acknowledged. */
  //GPS_cfg_ubxCfgTp5         = 0xB1,   /*!< Send msg ubxCfgTp5 */
  //GPS_cfgok_ubxCfgTp5       = 0xB2,   /*!< Message ubxCfgTp5 has been acknowledged. */
  //GPS_cfgNok_ubxCfgTp5      = 0xB3,   /*!< Message ubxCfgTp5 was not acknowledged. */
  GPS_cfg_DONE              = 0xFF,   /*!< Configuration has been performed sucessfully. */
} GPS_cgf_st_e;


/**    
*   \ingroup gs_ublox_cmd 
*
*/
/**
NMEA Protocol Specification:
2.3 
compatibility mode
enable SV consider mode
B5 62 06 17 04 00 00 23 00 03 47 55 */
const uint8_t GPS_ubxCfgNmea[12] = 
{0xB5 ,0x62 ,0x06 ,0x17 ,0x04 ,0x00 ,0x00 ,0x23 ,0x00 ,0x03 ,0x47 ,0x55};

/** UBX - MSG NMEA GPRMC
B5 62 06 01 08 00 F0 04 01 01 01 01 01 01 09 54 */
const uint8_t GPS_ubxCfgGprmc[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x04 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01 ,0x05 ,0x43};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x04 ,0x00 ,0x01 ,0x01 ,0x01 ,0x00 ,0x01 ,0x07 ,0x4C }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPVTG
B5 62 06 01 08 00 F0 05 01 00 00 01 01 01 08 52 */
const uint8_t GPS_ubxCfgGpvtg[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x05 ,0x01 ,0x00 ,0x00 ,0x01 ,0x01 ,0x01 ,0x08 ,0x52};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x05 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x0A ,0x5B }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPGGA
B5 62 06 01 08 00 F0 00 01 00 00 01 01 01 03 2F */
const uint8_t GPS_ubxCfgGpgga[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x00 ,0x01 ,0x00 ,0x00 ,0x01 ,0x01 ,0x01 ,0x03 ,0x2F};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x00 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x05 ,0x38 }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPGSA
B5 62 06 01 08 00 F0 02 01 00 00 01 01 01 05 3D */
const uint8_t GPS_ubxCfgGpgsa[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x02 ,0x01 ,0x00 ,0x00 ,0x01 ,0x01 ,0x01 ,0x05 ,0x3D};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x02 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x07 ,0x46 }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPGSV
B5 62 06 01 08 00 F0 03 01 00 00 01 01 01 06 44 */ 
const uint8_t GPS_ubxCfgGpgsv[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x03 ,0x01 ,0x00 ,0x00 ,0x01 ,0x01 ,0x01 ,0x06 ,0x44};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x03 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x08 ,0x4D }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPGLL
B5 62 06 01 08 00 F0 01 01 00 00 01 01 01 04 36 */
const uint8_t GPS_ubxCfgGpgll[16] = 
//{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x01 ,0x01 ,0x00 ,0x00 ,0x01 ,0x01 ,0x01 ,0x04 ,0x36};
{ 0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x06 ,0x3F }; //Activate UART1 and UART2 of the ublox module

/** UBX - MSG NMEA GPZDA 
B5 62 06 01 08 00 F0 08 00 00 00 00 00 00 07 5B */
const uint8_t GPS_ubxCfgGpzda[16] = 
{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x08 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x07 ,0x5b};

/**
CFG Timepulse
Time Source GPS time
B5 62 06 07 14 00 40 42 0F 00 A0 86 01 00 01 01 00 00 32 00 00 00 00 00 00 00 0D 6C 
*/
/*const uint8_t GPS_ubxCfgTp[28] = 
{0xB5 ,0x62 ,0x06 ,0x07 ,0x14 ,0x00 ,0x40 ,0x42 ,0x0F ,0x00 ,0xA0 ,0x86 ,0x01 ,0x00 ,0x01 ,0x01 
 ,0x00 ,0x00 ,0x32 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x0D ,0x6C};*/
const uint8_t GPS_ubxCfgTp[40] = 
{0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x40, 0x00, 0x40, 0x42, 
 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x20, 0xA1, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0xF3, 0x00, 0x00, 0x00, 0xA6, 0x8C};

/**
CFG Msg TP
B5 62 06 01 08 00 0D 01 00 00 00 01 00 00 1E 15 
*/
const uint8_t GPS_ubxCfgMsgTp[16] = 
{0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x0D ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x1F ,0x1A};

/**
CFG Msg UBX4
B5 62 06 01 08 00 F1 04 00 00 00 01 00 00 05 4A
*/
const uint8_t GPS_ubxCfgMsgUbx4[16] = 
{0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF1, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x4A};

/**
CFG Msg UBX-TP5
Time Source GPS time and GNSS time locked mode (get time correlate to time puls pps when no fix is avilable)
B5 62 06 31 20 00 00 00 00 00 32 00 40 00 40 42
0F 00 40 42 0F 00 20 A1 07 00 40 42 0F 00 00 00
00 00 F7 00 00 00 3B 08
*/
//const uint8_t GPS_ubxCfgTp5[40] =
//{0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x40, 0x00, 0x40, 0x42,
// 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x20, 0xA1, 0x07, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00,
// 0x00, 0x00, 0xF7, 0x00, 0x00, 0x00, 0x3B, 0x08};



/** \brief GPS NMEA sentence / message IDs */
typedef enum GPSmsgsID
{
  Invalid = 0,          /*!< Invalid string */
  GPAAM,            /*!< Waypoint Arrival Alarm */
  GPALM,            /*!< GPS Almanac Data */
  GPAPA,            /*!< Autopilot format "A" */
  GPAPB,            /*!< Autopilot format "B" */
  GPASD,            /*!< Autopilot System Data */
  GPBEC,            /*!< Bearing & Distance to Waypoint, Dead Reckoning */
  GPBOD,            /*!< Bearing, Origin to Destination */
  GPBWC,            /*!< Bearing & Distance to Waypoint, Great Circle */
  GPBWR,            /*!< Bearing & Distance to Waypoint, Rhumb Line */
  GPBWW,            /*!< Bearing, Waypoint to Waypoint */
  GPDBT,            /*!< Depth Below Transducer */
  GPDCN,            /*!< Decca Position */
  GPDPT,            /*!< Depth */
  GPFSI,            /*!< Frequency Set Information */
  GPGGA,            /*!< Global Positioning System Fix Data */
  GPGLC,            /*!< Geographic Position, Loran-C */
  GPGLL,            /*!< Geographic Position, Latitude/Longitude */
  GPGRS,            /*!< GPS Range Residuals */
  GPGSA,            /*!< GPS DOP and Active Satellites */
  GPGST,            /*!< GPS Pseudorange Noise Statistics */
  GPGSV,            /*!< GPS Satellites in View */
  GPGXA,            /*!< TRANSIT Position */
  GPHDG,            /*!< Heading, Deviation & Variation */
  GPHDT,            /*!< Heading, True */
  GPHSC,            /*!< Heading Steering Command */
  GPLCD,            /*!< Loran-C Signal Data */
  GPMSK,            /*!< Control for a Beacon Receiver */
  GPMSS,            /*!< Beacon Receiver Status */
  GPMTA,            /*!< Air Temperature (to be phased out) */
  GPMTW,            /*!< Water Temperature */
  GPMWD,            /*!< Wind Direction */
  GPMWV,            /*!< Wind Speed and Angle */
  GPOLN,            /*!< Omega Lane Numbers */
  GPOSD,            /*!< Own Ship Data */
  GPR00,            /*!< Waypoint active route (not standard) */
  GPRMA,            /*!< Recommended Minimum Specific Loran-C Data */
  GPRMB,            /*!< Recommended Minimum Navigation Information */
  GPRMC,            /*!< Recommended Minimum Specific GPS/TRANSIT Data */
  GPROT,            /*!< Rate of Turn */
  GPRPM,            /*!< Revolutions */
  GPRSA,            /*!< Rudder Sensor Angle */
  GPRSD,            /*!< RADAR System Data */
  GPRTE,            /*!< Routes */
  GPSFI,            /*!< Scanning Frequency Information */
  GPSTN,            /*!< Multiple Data ID */
  GPTRF,            /*!< TRANSIT Fix Data */
  GPTTM,            /*!< Tracked Target Message */
  GPVBW,            /*!< Dual Ground/Water Speed */
  GPVDR,            /*!< Set and Drift */
  GPVHW,            /*!< Water Speed and Heading */
  GPVLW,            /*!< Distance Traveled through the Water */
  GPVPW,            /*!< Speed, Measured Parallel to Wind */
  GPVTG,            /*!< Track Made Good and Ground Speed */
  GPWCV,            /*!< Waypoint Closure Velocity */
  GPWNC,            /*!< Distance, Waypoint to Waypoint */
  GPWPL,            /*!< Waypoint Location */
  GPXDR,            /*!< Transducer Measurements */
  GPXTE,            /*!< Cross-Track Error, Measured */
  GPXTR,            /*!< Cross-Track Error, Dead Reckoning */
  GPZDA,            /*!< UTC Date / Time and Local Time Zone Offset */
  GPZFO,            /*!< UTC & Time from Origin Waypoint */
  GPZTG,            /*!< UTC & Time to Destination Waypoint */ 
  PGRME,            /*!< Garmin Estimated Error Information non-standard */
  PGRMF,            /*!< Garmin GPS Fix Data Sentence non-standard */
  PGRMT,            /*!< Garmin Sensor Status Information non-standard */
  PGRMV,            /*!< Garmin 3D Velocity Information non-standard */
  PGRMI,            /*!< Garmin Initiallzation Information non-standard */
  PGRMC,            /*!< Garmin Configuration Information non-standard */
  PGRMO,             /*!< Garmin Output Sentence Enable/Disable non-standard */
  PUBX
} GPSMsgID_e;


/** \brief struct for converting a gps identification string to an enum */
typedef struct GPSMsgIDstring
{
	uint8_t  IDS[3];   /*!< 3 chars of the unique gps identification strings,  */
	uint8_t  EnumID;   /*!< GPS NMEA ID as byte (GPSMsgID_e) */
}	GPSMsgIDstring_t;


/** Array with possible identification strings for standard 
*   NMEA sentences starting with "GP".
*
*  \note Sorting of this array improves performance of the linear search.
*         RMC is sorted first, as it is the most important sentence.
*/
const GPSMsgIDstring_t NMEAIdentification[62] = 
{
  {{'R','M','C'}, (uint8_t)GPRMC},            /* Recommended Minimum Specific GPS/TRANSIT Data */
  {{'A','A','M'}, (uint8_t)GPAAM},            /* Waypoint Arrival Alarm */
  {{'A','L','M'}, (uint8_t)GPALM},            /* GPS Almanac Data */
  {{'A','P','A'}, (uint8_t)GPAPA},            /* Autopilot format "A" */
  {{'A','P','B'}, (uint8_t)GPAPB},            /* Autopilot format "B" */
  {{'A','S','D'}, (uint8_t)GPASD},            /* Autopilot System Data */
  {{'B','E','C'}, (uint8_t)GPBEC},            /* Bearing & Distance to Waypoint, Dead Reckoning */
  {{'B','O','D'}, (uint8_t)GPBOD},            /* Bearing, Origin to Destination */
  {{'B','W','C'}, (uint8_t)GPBWC},            /* Bearing & Distance to Waypoint, Great Circle */
  {{'B','W','R'}, (uint8_t)GPBWR},            /* Bearing & Distance to Waypoint, Rhumb Line */
  {{'B','W','W'}, (uint8_t)GPBWW},            /* Bearing, Waypoint to Waypoint */
  {{'D','B','T'}, (uint8_t)GPDBT},            /* Depth Below Transducer */
  {{'D','C','N'}, (uint8_t)GPDCN},            /* Decca Position */
  {{'D','P','T'}, (uint8_t)GPDBT},            /* Depth */
  {{'F','S','I'}, (uint8_t)GPFSI},            /* Frequency Set Information */
  {{'G','G','A'}, (uint8_t)GPGGA},            /* Global Positioning System Fix Data */
  {{'G','L','C'}, (uint8_t)GPGLC},            /* Geographic Position, Loran-C */
  {{'G','L','L'}, (uint8_t)GPGLL},            /* Geographic Position, Latitude/Longitude */
  {{'G','R','S'}, (uint8_t)GPGRS},            /* GPS Range Residuals */
  {{'G','S','A'}, (uint8_t)GPGSA},            /* GPS DOP and Active Satellites */
  {{'G','S','T'}, (uint8_t)GPGST},            /* GPS Pseudorange Noise Statistics */
  {{'G','S','V'}, (uint8_t)GPGSV},            /* GPS Satellites in View */
  {{'G','X','A'}, (uint8_t)GPGXA},            /* TRANSIT Position */
  {{'H','D','G'}, (uint8_t)GPHDG},            /* Heading, Deviation & Variation */
  {{'H','D','T'}, (uint8_t)GPHDT},            /* Heading, True */
  {{'H','S','C'}, (uint8_t)GPHSC},            /* Heading Steering Command */
  {{'L','C','D'}, (uint8_t)GPLCD},            /* Loran-C Signal Data */
  {{'M','S','K'}, (uint8_t)GPMSK},            /* Control for a Beacon Receiver */
  {{'M','S','S'}, (uint8_t)GPMSS},            /* Beacon Receiver Status */
  {{'M','T','A'}, (uint8_t)GPMTA},            /* Air Temperature (to be phased out) */
  {{'M','T','W'}, (uint8_t)GPMTW},            /* Water Temperature */
  {{'M','W','D'}, (uint8_t)GPMWD},            /* Wind Direction */
  {{'M','W','V'}, (uint8_t)GPMWV},            /* Wind Speed and Angle */
  {{'O','L','N'}, (uint8_t)GPOLN},            /* Omega Lane Numbers */
  {{'O','S','D'}, (uint8_t)GPOSD},            /* Own Ship Data */
  {{'R','0','0'}, (uint8_t)GPR00},            /* Waypoint active route (not standard) */
  {{'R','M','A'}, (uint8_t)GPRMA},            /* Recommended Minimum Specific Loran-C Data */
  {{'R','M','B'}, (uint8_t)GPRMB},            /* Recommended Minimum Navigation Information */
  {{'R','O','T'}, (uint8_t)GPROT},            /* Rate of Turn */
  {{'R','P','M'}, (uint8_t)GPRPM},            /* Revolutions */
  {{'R','S','A'}, (uint8_t)GPRSA},            /* Rudder Sensor Angle */
  {{'R','S','D'}, (uint8_t)GPRSD},            /* RADAR System Data */
  {{'R','T','E'}, (uint8_t)GPRTE},            /* Routes */
  {{'S','F','I'}, (uint8_t)GPSFI},            /* Scanning Frequency Information */
  {{'S','T','N'}, (uint8_t)GPSTN},            /* Multiple Data ID */
  {{'T','R','F'}, (uint8_t)GPTRF},            /* TRANSIT Fix Data */
  {{'T','T','M'}, (uint8_t)GPTTM},            /* Tracked Target Message */
  {{'V','B','W'}, (uint8_t)GPVBW},            /* Dual Ground/Water Speed */
  {{'V','D','R'}, (uint8_t)GPVDR},            /* Set and Drift */
  {{'V','H','W'}, (uint8_t)GPVHW},            /* Water Speed and Heading */
  {{'V','L','W'}, (uint8_t)GPVLW},            /* Distance Traveled through the Water */
  {{'V','P','W'}, (uint8_t)GPVPW},            /* Speed, Measured Parallel to Wind */
  {{'V','T','G'}, (uint8_t)GPVTG},            /* Track Made Good and Ground Speed */
  {{'W','C','V'}, (uint8_t)GPWCV},            /* Waypoint Closure Velocity */
  {{'W','N','C'}, (uint8_t)GPWNC},            /* Distance, Waypoint to Waypoint */
  {{'W','P','L'}, (uint8_t)GPWPL},            /* Waypoint Location */
  {{'X','D','R'}, (uint8_t)GPXDR},            /* Transducer Measurements */
  {{'X','T','E'}, (uint8_t)GPXTE},            /* Cross-Track Error, Measured */
  {{'X','T','R'}, (uint8_t)GPXTR},            /* Cross-Track Error, Dead Reckoning */
  {{'Z','D','A'}, (uint8_t)GPZDA},            /* UTC Date / Time and Local Time Zone Offset */
  {{'Z','F','O'}, (uint8_t)GPZFO},            /* UTC & Time from Origin Waypoint */
  {{'Z','T','G'}, (uint8_t)GPZTG}             /* UTC & Time to Destination Waypoint */
};

typedef struct GPStime
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hour;
  uint8_t   Min;
  uint8_t   Sec;
  uint16_t  mSec;
} GPSTime_t;

typedef struct  GPS_GPRMC
{
  union CntFlags
  {
    uint16_t All;
    struct
    {
      uint16_t  RMC_Valid : 1;
      uint16_t  TimeValid : 1;
      uint16_t  LockValid : 1;
      uint16_t  LatitudeValid : 1;
      uint16_t  LongitudeValid : 1;
      uint16_t  SpeedValid : 1;
      uint16_t  CourseValid : 1;
      uint16_t  DateValid : 1;
      uint16_t  MagVariationValid : 1;
      uint16_t  MagDirectionValid : 1;
      uint16_t  ChecksumValid : 1;
      uint16_t  IndexCnt : 5;
    } ValidField;
  } Validness;  /**< valid data fields of NMEA GPRMC string */

  uint16_t Position[20]; /**< start of data fields of NMEA GPRMC string */

} GPS_gprmc_t; //UTAGpsValidation_t;

struct GPSSyncData
{
    uint64_t gpsTime;
    uint64_t systemTime;
    uint16_t utcOffset;
};

#endif // GPSDEF_H
