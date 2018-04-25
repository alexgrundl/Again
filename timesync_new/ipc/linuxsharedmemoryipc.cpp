#ifdef __linux__

#include "linuxsharedmemoryipc.h"

LinuxSharedMemoryIPC::LinuxSharedMemoryIPC()
{
    shm_fd = 0;
    err = 0;
    master_offset_buffer = NULL;
    domain = 0;
}

LinuxSharedMemoryIPC::~LinuxSharedMemoryIPC()
{
    munmap(master_offset_buffer, SHM_SIZE);
    if(shm_unlink(shm_name.c_str()) != 0)
        printf("error shm_unlink: %s\n", strerror(errno));
}

bool LinuxSharedMemoryIPC::init( OS_IPC_ARG *barg )
{
    LinuxIPCArg *arg;
    struct group *grp;
    std::string group_name;
    pthread_mutexattr_t shared;
    mode_t oldumask = umask(0);

    if( barg == NULL )
    {
        group_name = DEFAULT_GROUPNAME;
    }
    else
    {
        arg = dynamic_cast<LinuxIPCArg *> (barg);
        if( arg == NULL )
        {
            fprintf(stderr, "Wrong IPC init arg type, File: %s\n", __FILE__);
            goto exit_error;
        }
        else
        {
            group_name = arg->group_name.length() > 0 ? arg->group_name : DEFAULT_GROUPNAME;
            if_name = arg->if_name;
            domain = arg->domain;
        }
    }
    grp = getgrnam( group_name.c_str() );
    if( grp == NULL )
    {
        //GPTP_LOG_ERROR( "Group %s not found, will try root (0) instead", group_name.c_str() );
    }

    OS_IPC::GetShmName(if_name, domain, shm_name);
    shm_fd = shm_open( shm_name.c_str(), O_RDWR | O_CREAT, 0660 );
    if( shm_fd == -1 )
    {
        fprintf(stderr, "shm_open(): %s, File: %s\n", strerror(errno), __FILE__);
        goto exit_error;
    }
    (void) umask(oldumask);
    if (fchown(shm_fd, -1, grp != NULL ? grp->gr_gid : 0) < 0)
    {
        fprintf(stderr, "shm_open(): Failed to set ownership, File: %s\n", __FILE__);
    }
    if( ftruncate( shm_fd, SHM_SIZE ) == -1 )
    {
        fprintf(stderr, "ftruncate(), File: %s\n", __FILE__);
        goto exit_unlink;
    }
    master_offset_buffer = (char *) mmap
        ( NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_LOCKED | MAP_SHARED,
          shm_fd, 0 );
    if( master_offset_buffer == (char *) -1 )
    {
        fprintf(stderr, "mmap(), File: %s\n", __FILE__);
        goto exit_unlink;
    }
    /*create mutex attr */
    err = pthread_mutexattr_init(&shared);
    if(err != 0)
    {
        fprintf(stderr, "mutex attr initialization failed - %s, File: %s\n", strerror(errno), __FILE__);
        goto exit_unlink;
    }
    pthread_mutexattr_setpshared(&shared,1);
    /*create a mutex */
    err = pthread_mutex_init((pthread_mutex_t *) master_offset_buffer, &shared);
    if(err != 0)
    {
        fprintf(stderr, "sharedmem - Mutex initialization failed - %s, File: %s\n", strerror(errno), __FILE__);
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

bool LinuxSharedMemoryIPC::update(int64_t ml_phoffset,
    int64_t ls_phoffset,
    FrequencyRatio ml_freqoffset,
    FrequencyRatio ls_freqoffset,
    uint64_t local_time,
    uint32_t sync_count,
    uint32_t pdelay_count,
    PortState port_state,
    bool asCapable,
    SystemTimeBase system_time_base , int64_t pdelay)
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
//		ptimedata->process_id   = process_id; // set once on init now
        /* unlock */
        pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
    }
    return true;
}
void LinuxSharedMemoryIPC::setpid()
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

void LinuxSharedMemoryIPC::set_ifname(const char* ifname)
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
void LinuxSharedMemoryIPC::set_if_phc_index(int phc_idx)
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


bool LinuxSharedMemoryIPC::update_grandmaster(
    uint8_t gptp_grandmaster_id[],
    uint8_t gptp_domain_number )
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    if( shm_buffer != NULL ) {
        /* lock */
        pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
        buf_offset += sizeof(pthread_mutex_t);
        ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
        memcpy(ptimedata->gptp_grandmaster_id, gptp_grandmaster_id, PTP_CLOCK_IDENTITY_LENGTH);
        ptimedata->gptp_domain_number = gptp_domain_number;
        /* unlock */
        pthread_mutex_unlock((pthread_mutex_t *) shm_buffer);
    }
    return true;
}

bool LinuxSharedMemoryIPC::update_network_interface(
    const uint8_t  clock_identity[],
    uint8_t  priority1,
    uint8_t  clock_class,
    int16_t  offset_scaled_log_variance,
    uint8_t  clock_accuracy,
    uint8_t  priority2,
    uint8_t  domain_number,
    int8_t   log_sync_interval,
    int8_t   log_announce_interval,
    int8_t   log_pdelay_interval,
    uint16_t port_number )
{
    int buf_offset = 0;
    char *shm_buffer = master_offset_buffer;
    gPtpTimeData *ptimedata;
    if( shm_buffer != NULL ) {
        /* lock */
        pthread_mutex_lock((pthread_mutex_t *) shm_buffer);
        buf_offset += sizeof(pthread_mutex_t);
        ptimedata   = (gPtpTimeData *) (shm_buffer + buf_offset);
        memcpy(ptimedata->clock_identity, clock_identity, PTP_CLOCK_IDENTITY_LENGTH);
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

void LinuxSharedMemoryIPC::stop() {
    if( master_offset_buffer != NULL ) {
        munmap( master_offset_buffer, SHM_SIZE );
        shm_unlink( shm_name.c_str() );
    }
}

#endif
