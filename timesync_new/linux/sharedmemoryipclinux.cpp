#ifdef __linux__

#include "sharedmemoryipclinux.h"

IPCLinux::IPCLinux()
{
    shm_fd = 0;
    err = 0;
    master_offset_buffer = NULL;
    domain = 0;
}

IPCLinux::~IPCLinux()
{
    munmap(master_offset_buffer, SHM_SIZE);
    if(shm_unlink(shm_name.c_str()) != 0)
        logerror("error shm_unlink: %s\n", strerror(errno));
}

bool IPCLinux::init(std::string if_name, unsigned char domain)
{
    this->if_name = if_name;
    this->domain = domain;
    pthread_mutexattr_t shared;
    mode_t oldumask = umask(0);

    GetShmName(if_name, domain, shm_name);
    shm_fd = shm_open( shm_name.c_str(), O_RDWR | O_CREAT, 0660 );
    if( shm_fd == -1 )
    {
        logerror("shm_open(): %s, File: %s\n", strerror(errno), __FILE__);
        goto exit_error;
    }
    (void) umask(oldumask);
    if (fchown(shm_fd, -1, 0) < 0)
    {
        logerror("shm_open(): Failed to set ownership, File: %s\n", __FILE__);
    }
    if( ftruncate( shm_fd, SHM_SIZE ) == -1 )
    {
        logerror("ftruncate(), File: %s\n", __FILE__);
        goto exit_unlink;
    }
    master_offset_buffer = (char *) mmap
        ( NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_LOCKED | MAP_SHARED,
          shm_fd, 0 );
    if( master_offset_buffer == (char *) -1 )
    {
        logerror("mmap(), File: %s\n", __FILE__);
        goto exit_unlink;
    }
    /*create mutex attr */
    err = pthread_mutexattr_init(&shared);
    if(err != 0)
    {
        logerror("mutex attr initialization failed - %s, File: %s\n", strerror(errno), __FILE__);
        goto exit_unlink;
    }
    pthread_mutexattr_setpshared(&shared,1);
    /*create a mutex */
    err = pthread_mutex_init((pthread_mutex_t *) master_offset_buffer, &shared);
    if(err != 0)
    {
        logerror("sharedmem - Mutex initialization failed - %s, File: %s\n", strerror(errno), __FILE__);
        goto exit_unlink;
    }
    setpid();
    set_if_phc_index(-1); // unknown at the moment
    return true;
 exit_unlink:
    shm_unlink( shm_name.c_str() );
 exit_error:
    return false;
}

bool IPCLinux::update(int64_t ml_phoffset,
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
    GpsClockState gps_clock_state)
{
    int buf_offset = 0;
//	pid_t process_id = getpid();
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    if( shm_buffer != NULL ) {
        /* lock */
        pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
        buf_offset += sizeof(pthread_mutex_t);
        ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
        ptimedata->ml_phoffset = ml_phoffset;
        ptimedata->ls_phoffset = ls_phoffset;
        ptimedata->ml_freqoffset = ml_freqoffset;
        ptimedata->ls_freqoffset = ls_freqoffset;
        ptimedata->local_time = local_time;
        ptimedata->sync_count   = sync_count;
        ptimedata->pdelay_count = pdelay_count;
        ptimedata->asCapable = asCapable;
        ptimedata->port_state   = port_state;
        ptimedata->system_time_base = system_time_base;
        ptimedata->pdelay = pdelay;
        ptimedata->grandmaster_clock_time_source = clock_time_source;
        ptimedata->gps_status = gps_clock_state;
//		ptimedata->process_id   = process_id; // set once on init now
        /* unlock */
        pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
    }
    return true;
}
void IPCLinux::setpid()
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;

    pid_t process_id = getpid();
    pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
    buf_offset += sizeof(pthread_mutex_t);
    ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
    ptimedata->process_id   = process_id;
    pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
}

void IPCLinux::set_ifname(const char* ifname)
{

    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
    buf_offset += sizeof(pthread_mutex_t);
    ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
    memset(ptimedata->ifname, 0, ARRAY_SIZE(ptimedata->ifname));
    memcpy(ptimedata->ifname, ifname, ARRAY_SIZE(ptimedata->ifname)-1);
    pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
}
void IPCLinux::set_if_phc_index(int phc_idx)
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
    buf_offset += sizeof(pthread_mutex_t);
    ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
    ptimedata->phc_index=phc_idx;
    pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
}


bool IPCLinux::update_grandmaster(const uint8_t  clock_identity[],
    uint8_t  priority1,
    uint8_t  clock_class,
    int16_t  offset_scaled_log_variance,
    ClockAccuracy clock_accuracy,
    uint8_t  priority2)
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    if( shm_buffer != NULL ) {
        /* lock */
        pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
        buf_offset += sizeof(pthread_mutex_t);
        ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
        memcpy(ptimedata->grandmaster_id, clock_identity, CLOCK_ID_LENGTH);

        ptimedata->grandmaster_priority1 = priority1;
        ptimedata->grandmaster_clock_class = clock_class;
        ptimedata->grandmaster_offset_scaled_log_variance = offset_scaled_log_variance;
        ptimedata->grandmaster_clock_accuracy = clock_accuracy;
        ptimedata->grandmaster_priority2 = priority2;
        /* unlock */
        pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
    }
    return true;
}

bool IPCLinux::update_network_interface(const uint8_t  clock_identity[],
    uint8_t  priority1,
    uint8_t  clock_class,
    int16_t  offset_scaled_log_variance,
    ClockAccuracy clock_accuracy,
    uint8_t  priority2,
    uint8_t  domain_number,
    int8_t   log_sync_interval,
    int8_t   log_announce_interval,
    int8_t   log_pdelay_interval,
    uint16_t port_number)
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    if( shm_buffer != NULL ) {
        /* lock */
        pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
        buf_offset += sizeof(pthread_mutex_t);
        ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
        memcpy(ptimedata->clock_identity, clock_identity, CLOCK_ID_LENGTH);
        ptimedata->priority1 = priority1;
        ptimedata->clock_class = clock_class;
        ptimedata->offset_scaled_log_variance = offset_scaled_log_variance;
        ptimedata->clock_accuracy = clock_accuracy;
        ptimedata->priority2 = priority2;
        ptimedata->domain_number = domain_number;
        ptimedata->log_sync_interval = log_sync_interval;
        ptimedata->log_announce_interval = log_announce_interval;
        ptimedata->log_pdelay_interval = log_pdelay_interval;
        ptimedata->port_number   = port_number;
        /* unlock */
        pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
    }
    return true;
}

void IPCLinux::stop() {
    if( master_offset_buffer != NULL ) {
        munmap( master_offset_buffer, SHM_SIZE );
        shm_unlink( shm_name.c_str() );
    }
}

#endif
