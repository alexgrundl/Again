#ifndef IPCDEF_HPP
#define IPCDEF_HPP

/**@file
 * This is a common header file. OS-specific implementations should use
 * this file as base. Currently we have two IPC implementations:
 * Linux: Located at linux/src/linux_ipc.hpp (among other files that include this)
 * Windows: Located at windows/daemon_cl/windows_ipc.hpp
*/

#include <stdbool.h>
#include <string>
#include <sstream>

#if defined (__unix__) || defined(__linux__)
#include <sys/types.h>

/*Type for process id*/
#define PID_TYPE    pid_t

#elif defined(_WIN32) || defined(_WIN64)

/*Definition of DWORD*/
#include <IntSafe.h>

/*Type for process ID*/
#define PID_TYPE    DWORD

#else
/*Create new ifdefs for different OSs and/or add to the existing ones*/
#error "ERROR. OS not supported"
#endif /*__unix__ / _WIN*/

#if defined(__clang__) &&  defined(__x86_64__)
// Clang/llvm has incompatible long double (fp128) for x86_64.
typedef double FrequencyRatio;		/*!< Frequency Ratio */
#else
typedef long double FrequencyRatio;	/*!< Frequency Ratio */
#endif

/* A clock that is synchronized to a primary reference time source. */
#define CLOCK_CLASS_PRIMARY 6
/* A clock that has previously been designated as CLOCK_CLASS_PRIMARY but that has lost the ability to synchronize. */
#define CLOCK_CLASS_PRIMARY_LOST 7
/* A clock that is synchronized to an application-specific source of time. */
#define CLOCK_CLASS_APPLICATION 13
/* A clock that has previously been designated as CLOCK_CLASS_APPLICATION but that has lost the ability to synchronize to an application-specific source of time and
 * is in holdover mode and within holdover specifications. */
#define CLOCK_CLASS_APPLICATION_LOST 14
/* 52: Degradation alternative A for a clock of clockClass 7 that is not within holdover specification */
#define CLOCK_CLASS_PRIMARY_LOST_DEGRADED_A 52
/* Degradation alternative A for a clock of clockClass 14 that is not within holdover specification */
#define CLOCK_CLASS_APPLICATION_LOST_DEGRADED_A 58

//68..122: For use by alternate PTP profiles (68..122),
//133..170: For use by alternate PTP profiles (133..170),

/* Degradation alternative B for a clock of clockClass 7 that is not within holdover specification */
#define CLOCK_CLASS_PRIMARY_LOST_DEGRADED_B 187
/* Degradation alternative B for a clock of clockClass 14 that is not within holdover specification */
#define CLOCK_CLASS_APPLICATION_LOST_DEGRADED_B 193

//    216..232: For use by alternate PTP profiles,

/* Default none of the other clockClass definitions apply */
#define CLOCK_CLASS_DEFAULT 248
/* A slave-only clock */
#define CLOCK_CLASS_SLAVE_ONLY 255


#define CLOCK_ID_LENGTH 8

typedef enum
{
    CLOCK_ACCURACY_NS_25 = 32,         // The time is accurate to within 25 ns.
    CLOCK_ACCURACY_NS_100 = 33,        // The time is accurate to within 100 ns.
    CLOCK_ACCURACY_NS_250 = 34,        // The time is accurate to within 250 ns.
    CLOCK_ACCURACY_NS_1000 = 35,       // The time is accurate to within 1000 ns.
    CLOCK_ACCURACY_NS_2500 = 36,       // The time is accurate to within 2500 ns.

    CLOCK_ACCURACY_US_25 = 37,         // The time is accurate to within 25 us.
    CLOCK_ACCURACY_US_100 = 38,        // The time is accurate to within 100 us.
    CLOCK_ACCURACY_US_250 = 39,        // The time is accurate to within 250 us.
    CLOCK_ACCURACY_US_1000 = 40,       // The time is accurate to within 1000 us.
    CLOCK_ACCURACY_US_2500 = 41,       // The time is accurate to within 2500 us.

    CLOCK_ACCURACY_MS_25 = 42,         // The time is accurate to within 25 ms.
    CLOCK_ACCURACY_MS_100 = 43,        // The time is accurate to within 100 ms.
    CLOCK_ACCURACY_MS_250 = 44,        // The time is accurate to within 250 ms.

    CLOCK_ACCURACY_S_1 = 45,           // The time is accurate to within 1 s.
    CLOCK_ACCURACY_S_10 = 46,          // The time is accurate to within 10 s.
    CLOCK_ACCURACY_S_GREATER_10 = 46,  // The time is accurate to within 10 s.
    CLOCK_ACCURACY_UNKNOWN = 254       // Default indicating unknown
} ClockAccuracy;

/**
 * @brief PortState enumeration
 */
typedef enum
{
    PTP_UNKNOWN = 0,
    PTP_MASTER = 6,     //!< Port is PTP Master
    PTP_PRE_MASTER,     //!< Port is not PTP Master yet.
    PTP_SLAVE,          //!< Port is PTP Slave
    PTP_UNCALIBRATED,   //!< Port is uncalibrated.
    PTP_DISABLED,       //!< Port is not PTP enabled. All messages are ignored when in this state.
    PTP_FAULTY,         //!< Port is in a faulty state. Recovery is implementation specific.
    PTP_INITIALIZING,   //!< Port's initial state.
    PTP_LISTENING       //!< Port is in a PTP listening state. Currently not in use.
} PortState;


typedef enum
{
    GPTP_DEFAULT=0,
    GPTP_CLOCK_REALTIME,
    GPTP_CLOCK_MONOTONIC_RAW
} SystemTimeBase;

typedef enum
{
    GPS_CLOCK_STATE_UNKNOWN = 0,
    GPS_CLOCK_STATE_INTERNAL = 1,
    GPS_CLOCK_STATE_AVAILABLE = 2
} GpsClockState;

typedef enum
{
    /**
     * Any device, or device directly connected to such a device, that is based on atomic resonance for frequency and that has been calibrated
     * against international standards for frequency and time.
     */
    CLOCK_TIME_SOURCE_ATOMIC = 0x10,

    /**
     * Any device synchronized to any of the satellite systems that distribute time and frequency tied to international standards.
     */
    CLOCK_TIME_SOURCE_GPS = 0x20,
    /**
     * Any device synchronized via any of the radio distribution systems that distribute time and frequency tied to international standards.
     */
    CLOCK_TIME_SOURCE_TERRESTRIAL_RADIO = 0x30,
    /**
     * Any device synchronized to an IEEE 1588 PTP-based source of time external to the gPTP domain.
     */
    CLOCK_TIME_SOURCE_PTP = 0x40,
    /**
     * Any device synchronized via NTP to servers that distribute time and frequency tied to international standards.
     */
    CLOCK_TIME_SOURCE_NTP = 0x50,
    /**
     * Used in all cases for any device whose time has been set by means of a human interface based on observation of an international standards
     * source of time to within the claimed clock accuracy.
     */
    CLOCK_TIME_SOURCE_HAND_SET = 0x60,
    /**
     * Any source of time and/or frequency not covered by other values, or for which the source is not known.
     */
    CLOCK_TIME_SOURCE_OTHER = 0x90,
    /**
     * Any device whose frequency is not based on atomic resonance nor calibrated against international standards for frequency, and whose time
     * is based on a free-running oscillator with epoch determined in an arbitrary or unknown manner.
     */
    CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR = 0xA0,
} ClockTimeSource;

/**
 * @brief Provides a data structure for gPTP time
 */
typedef struct {
    int64_t ml_phoffset;            //!< Master to local phase offset
    int64_t ls_phoffset;            //!< Local to system phase offset
    FrequencyRatio ml_freqoffset;   //!< Master to local frequency offset
    FrequencyRatio ls_freqoffset;   //!< Local to system frequency offset
    uint64_t local_time;            //!< Local time of last update

    /* Current grandmaster information */
    /* Referenced by the IEEE Std 1722.1-2013 AVDECC Discovery Protocol Data Unit (ADPDU) */
    uint8_t grandmaster_id[CLOCK_ID_LENGTH];	//!< Current grandmaster id (all 0's if no grandmaster selected)
    uint8_t grandmaster_priority1;
    uint8_t grandmaster_clock_class;
    uint16_t grandmaster_offset_scaled_log_variance;
    uint8_t grandmaster_clock_accuracy;
    uint8_t grandmaster_priority2;

    /* Grandmaster support for the network interface */
    /* Referenced by the IEEE Std 1722.1-2013 AVDECC AVB_INTERFACE descriptor */
    uint8_t  clock_identity[CLOCK_ID_LENGTH];	//!< The clock identity of the interface
    uint8_t  priority1;             //!< The priority1 field of the grandmaster functionality of the interface, or 0xFF if not supported
    uint8_t  clock_class;           //!< The clockClass field of the grandmaster functionality of the interface, or 0xFF if not supported
    uint16_t  offset_scaled_log_variance;    //!< The offsetScaledLogVariance field of the grandmaster functionality of the interface, or 0x0000 if not supported
    uint8_t  clock_accuracy;        //!< The clockAccuracy field of the grandmaster functionality of the interface, or 0xFF if not supported
    uint8_t  priority2;             //!< The priority2 field of the grandmaster functionality of the interface, or 0xFF if not supported
    uint8_t  domain_number;         //!< The domainNumber field of the grandmaster functionality of the interface, or 0 if not supported
    int8_t   log_sync_interval;     //!< The currentLogSyncInterval field of the grandmaster functionality of the interface, or 0 if not supported
    int8_t   log_announce_interval; //!< The currentLogAnnounceInterval field of the grandmaster functionality of the interface, or 0 if not supported
    int8_t   log_pdelay_interval;   //!< The currentLogPDelayReqInterval field of the grandmaster functionality of the interface, or 0 if not supported
    uint16_t port_number;           //!< The portNumber field of the interface, or 0x0000 if not supported

    /* Linux-specific */
    uint32_t sync_count;            //!< Sync messages count
    uint32_t pdelay_count;          //!< pdelay messages count
    bool asCapable;                 //!< asCapable flag: true = device is AS Capable; false otherwise
    PortState port_state;           //!< gPTP port state. It can assume values defined at ::PortState
    PID_TYPE process_id;            //!< Process id number

    /* bplus-specific LINUX*/
    SystemTimeBase system_time_base; //!< While get SystemTime in HWTimestamper_gettime(), which is system time counter is used: CLOCK_REALTIME(gettimeofday)[default], or if avalaible CLOCK_MONOTONIC_RAW (PTP Layer in Kernel needs specific b-plus patch for related ioctl)
    char ifname[16];                 //!< name of used network interface (like. eth0, enp15s0, etc.)
    int  phc_index;                  //!< phc index of used network interface (ptp clock index) (e.g. /dev/ptp4)
    int64_t pdelay;
    ClockTimeSource grandmaster_clock_time_source;
    GpsClockState gps_status;
} gPtpTimeData;


#define SHM_NAME_DEFAULT  "/ptp"    /*!< Start of every shared memory name*/
#define SHM_SIZE (sizeof(gPtpTimeData) + sizeof(pthread_mutex_t))   /*!< Shared memory size*/

class IPC
{
public:

    virtual bool init(std::string if_name, unsigned char  domain) = 0;


    virtual bool update(int64_t ml_phoffset,
        int64_t ls_phoffset,
        FrequencyRatio ml_freqoffset,
        FrequencyRatio ls_freqoffset,
        uint64_t local_time,
        uint32_t sync_count,
        uint32_t pdelay_count,
        PortState port_state,
        bool asCapable,
        SystemTimeBase system_time_base,
        int64_t pdelay,
        ClockTimeSource clock_time_source,
        GpsClockState gps_clock_state) = 0;


    virtual bool update_grandmaster(const uint8_t  clock_identity[],
                                    uint8_t  priority1,
                                    uint8_t  clock_class,
                                    int16_t  offset_scaled_log_variance,
                                    ClockAccuracy clock_accuracy,
                                    uint8_t  priority2) = 0;


    virtual bool update_network_interface(
        const uint8_t  clock_identity[],
        uint8_t  priority1,
        uint8_t  clock_class,
        int16_t  offset_scaled_log_variance,
        ClockAccuracy  clock_accuracy,
        uint8_t  priority2,
        uint8_t  domain_number,
        int8_t   log_sync_interval,
        int8_t   log_announce_interval,
        int8_t   log_pdelay_interval,
        uint16_t port_number) = 0;


    /**
     * @brief  Gets the name of shared memory depending on the name of
     * the ethernet interface and the domain. Only useful in linux. In
     * Windows nothing is done.
     * @param if_name The name of the ethernet interface (e.g. eth1)
     * @param domain The current domain
     * @shm_name Will contain the name of the shared memory
     */
    static void GetShmName(std::string if_name, int domain, std::string& shm_name)
    {
        #ifdef __linux__
            std::stringstream ss;
            ss << SHM_NAME_DEFAULT << "_" << if_name << "_" << domain;
            shm_name = ss.str();
        #endif
    }
};
/*

   Integer64  <master-local phase offset>
   Integer64  <local-system phase offset>
   LongDouble <master-local frequency offset>
   LongDouble <local-system frequency offset>
   UInteger64 <local time of last update>

 * Meaning of IPC provided values:

 master  ~= local   - <master-local phase offset>
 local   ~= system  - <local-system phase offset>
 Dmaster ~= Dlocal  * <master-local frequency offset>
 Dlocal  ~= Dsystem * <local-system freq offset>        (where D denotes a delta)

*/

#endif/*IPCDEF_HPP*/
