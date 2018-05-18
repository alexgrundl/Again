#ifndef TYPES_H
#define TYPES_H

#define __COMMONDEFS_LOGTAGS "<xtss-daemon v2>"
#include "commondefs.h"

#include <stdint.h>
#include <cstddef>
#include <math.h>
#include <stdio.h>

#include "scaledns.h"
#include "uscaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

#define CLOCK_ID_LENGTH 8

typedef ScaledNs TimeInterval;

struct PortIdentity
{
    uint8_t clockIdentity[CLOCK_ID_LENGTH];
    uint16_t portNumber;
};

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


enum ClockAccuracy
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
};

struct ClockQuality
{
    uint8_t clockClass;
    ClockAccuracy clockAccuracy;
    uint16_t offsetScaledLogVariance;
};

#define TIME_AWARE_SYSTEM_NETWORK  246 // Network infrastructure time-aware system
#define TIME_AWARE_SYSTEM_OTHER    248 // Other time-aware systems
#define TIME_AWARE_SYSTEM_PORTABLE 250 // Portable time-aware system
#define TIME_AWARE_SYSTEM_SLAVE    255 // Slave only time-aware system

enum ClockTimeSource
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

};

enum PortRole
{
    /**
     * Any port of the time-aware system for which portEnabled, pttPortEnabled, and asCapable are not all TRUE.
     */
    PORT_ROLE_DISABLED = 3,
    /**
     * Any port, P, of the time-aware system that is closer to the root than any other port of the gPTP communication path connected to P
     */
    PORT_ROLE_MASTER = 6,
    /**
     * Any port of the time-aware system whose port role is not MasterPort, SlavePort, or DisabledPort.
     */
    PORT_ROLE_PASSIVE = 7,
    /**
     * The one port of the time-aware system that is closest to the root time-aware system. If the root is grandmaster-capable,
     * the SlavePort is also closest to the grandmaster. The time- aware system does not transmit Sync or Announce messages on the SlavePort.
     */
    PORT_ROLE_SLAVE = 9
};

typedef UScaledNs MDTimestampReceive;


struct SystemIdentity
{
    uint8_t priority1;
    ClockQuality clockQuality;
    uint8_t priority2;
    uint8_t clockIdentity[CLOCK_ID_LENGTH];

    enum Info
    {
        INFO_SUPERIOR,
        INFO_EQUAL,
        INFO_INFERIOR
    };


    Info Compare(SystemIdentity identity)
    {
        if(priority1 == identity.priority1)
        {
            if(clockQuality.clockClass == identity.clockQuality.clockClass)
            {
                if(clockQuality.clockAccuracy == identity.clockQuality.clockAccuracy)
                {
                    if(clockQuality.offsetScaledLogVariance == identity.clockQuality.offsetScaledLogVariance)
                    {
                        if(priority2 == identity.priority2)
                        {
                            for (uint8_t i = 0; i < sizeof(clockIdentity); ++i)
                            {
                                if(clockIdentity[i] != identity.clockIdentity[i])
                                    return clockIdentity[i] < identity.clockIdentity[i] ? INFO_SUPERIOR : INFO_INFERIOR;
                            }
                            return INFO_EQUAL;
                        }
                        return priority2 < identity.priority2 ? INFO_SUPERIOR : INFO_INFERIOR;
                    }
                    return clockQuality.offsetScaledLogVariance < identity.clockQuality.offsetScaledLogVariance ? INFO_SUPERIOR : INFO_INFERIOR;
                }
                return clockQuality.clockAccuracy < identity.clockQuality.clockAccuracy ? INFO_SUPERIOR : INFO_INFERIOR;
            }
            return clockQuality.clockClass < identity.clockQuality.clockClass ? INFO_SUPERIOR : INFO_INFERIOR;
        }
        return priority1 < identity.priority1 ? INFO_SUPERIOR : INFO_INFERIOR;
    }
};

struct PriorityVector
{
    SystemIdentity identity;
    uint16_t stepsRemoved;

    /**
     * @brief The port identity of the transmitting time-aware-system
     */
    PortIdentity sourcePortIdentity;

    /**
     * @brief Port number of the receiving port.
     */
    uint16_t portNumber;


    SystemIdentity::Info Compare(PriorityVector vector)
    {
        SystemIdentity::Info info = identity.Compare(vector.identity);
        if(info == SystemIdentity::INFO_EQUAL)
        {
            if(stepsRemoved == vector.stepsRemoved)
            {
                for (uint8_t i = 0; i < sizeof(sourcePortIdentity.clockIdentity); ++i)
                {
                    if(sourcePortIdentity.clockIdentity[i] != vector.sourcePortIdentity.clockIdentity[i])
                        return sourcePortIdentity.clockIdentity[i] < vector.sourcePortIdentity.clockIdentity[i] ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
                }
                if(sourcePortIdentity.portNumber == vector.sourcePortIdentity.portNumber)
                {
                    if(portNumber != vector.portNumber)
                        return portNumber < vector.portNumber ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
                    return SystemIdentity::INFO_EQUAL;
                }
                return sourcePortIdentity.portNumber < vector.sourcePortIdentity.portNumber ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
            }
            return stepsRemoved < vector.stepsRemoved ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
        }
        return info;
    }
};

enum SpanningTreePortState
{
    /**
     * The port has received current information (i.e., announce receipt timeout has not occurred and, if gmPresent is TRUE, sync receipt timeout
     * also has not occurred) from the master time-aware system for the attached communication path.
     */
    SPANNING_TREE_PORT_STATE_RECEIVED,
    /**
     * Information for the port has been derived from the SlavePort for the time-aware system (with the addition of SlavePort stepsRemoved).
     * This includes the possibility that the SlavePort is the port whose portNumber is 0, i.e., the time-aware system is the root of the
     * gPTP domain.
     */
    SPANNING_TREE_PORT_STATE_MINE,
    /**
     * Announce receipt timeout or, in the case where gmPresent is TRUE, sync receipt timeout have occurred.
     */
    SPANNING_TREE_PORT_STATE_AGED,
    /**
     * PortEnabled, pttPortEnabled, and asCapable are not all TRUE.
     */
    SPANNING_TREE_PORT_STATE_DISABLED
};

#endif // TYPES_H

