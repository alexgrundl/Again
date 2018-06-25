#ifndef LINUXSHAREDMEMORYIPC_H
#define LINUXSHAREDMEMORYIPC_H

#ifdef __linux__

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>
#include <sstream>

#include "types.h"
#include "GPSdef.h"
#include "ipcdef.hpp"

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/**
 * @brief Linux shared memory interface
 */
class IPCLinux : IPC
{

private:
    int shm_fd;
    char *master_offset_buffer;
    int err;
    std::string if_name;
    unsigned char domain;
    std::string shm_name;
    void setpid();

public:
    /**
     * @brief Initializes the internal flags
     */
    IPCLinux();
    /**
     * @brief Destroys and unlinks shared memory
     */
    virtual ~IPCLinux();

    /**
     * @brief Initializes shared memory
     * @param if_name The interface the shared memory is used on.
     * @return TRUE if no error, FALSE otherwise
     */
    virtual bool init(std::string if_name, unsigned char  domain);

    /**
     * @brief Updates IPC values
     *
     * @param ml_phoffset Master to local phase offset
     * @param ls_phoffset Local to slave phase offset
     * @param ml_freqoffset Master to local frequency offset
     * @param ls_freqoffset Local to slave frequency offset
     * @param local_time Local time
     * @param sync_count Count of syncs
     * @param pdelay_count Count of pdelays
     * @param port_state Port's state
     * @param asCapable asCapable flag
     *
     * @return TRUE
     */
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
        GpsClockState gps_clock_state);

    /**
     * @brief Updates grandmaster IPC values
     *
     * @param gptp_grandmaster_id Current grandmaster id (all 0's if no grandmaster selected)
     * @param gptp_domain_number gPTP domain number
     *
     * @return TRUE
     */
    virtual bool update_grandmaster(const uint8_t  clock_identity[],
                                    uint8_t  priority1,
                                    uint8_t  clock_class,
                                    int16_t  offset_scaled_log_variance,
                                    ClockAccuracy clock_accuracy,
                                    uint8_t  priority2);

    /**
     * @brief Updates network interface IPC values
     *
     * @param  clock_identity  The clock identity of the interface
     * @param  priority1  The priority1 field of the grandmaster functionality of the interface, or 0xFF if not supported
     * @param  clock_class  The clockClass field of the grandmaster functionality of the interface, or 0xFF if not supported
     * @param  offset_scaled_log_variance  The offsetScaledLogVariance field of the grandmaster functionality of the interface, or 0x0000 if not supported
     * @param  clock_accuracy  The clockAccuracy field of the grandmaster functionality of the interface, or 0xFF if not supported
     * @param  priority2  The priority2 field of the grandmaster functionality of the interface, or 0xFF if not supported
     * @param  domain_number  The domainNumber field of the grandmaster functionality of the interface, or 0 if not supported
     * @param  log_sync_interval  The currentLogSyncInterval field of the grandmaster functionality of the interface, or 0 if not supported
     * @param  log_announce_interval  The currentLogAnnounceInterval field of the grandmaster functionality of the interface, or 0 if not supported
     * @param  log_pdelay_interval  The currentLogPDelayReqInterval field of the grandmaster functionality of the interface, or 0 if not supported
     * @param  port_number  The portNumber field of the interface, or 0x0000 if not supported
     *
     * @return TRUE
     */
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
        uint16_t port_number);

    /**
     * @brief unmaps and unlink shared memory
     * @return void
     */
    void stop();


    void set_ifname(const char* ifname);


    void set_if_phc_index(int phc_idx);
};


#endif

#endif // LINUXSHAREDMEMORYIPC_H
